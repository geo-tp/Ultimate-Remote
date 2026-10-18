#!/usr/bin/env python3
"""Fetch the public Flipper-IRDB catalog and cache selected remotes. Standard library only."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import sys
import tempfile
import urllib.error
import urllib.request

API = "https://api.github.com/repos/Lucaslhm/Flipper-IRDB"
MAX_INDEX = 16 * 1024 * 1024
MAX_REMOTE = 4 * 1024 * 1024
SHA = re.compile(r"[0-9a-f]{40}\Z")


class CatalogError(Exception):
    pass


def safe_path(path):
    return (isinstance(path, str) and 0 < len(path) <= 1024 and
            not path.startswith('/') and '\\' not in path and
            all(ord(c) >= 32 and ord(c) != 127 for c in path) and
            all(part not in ('', '.', '..') and not part.startswith('.') for part in path.split('/')))


def blob_sha(data):
    return hashlib.sha1(b'blob ' + str(len(data)).encode() + b'\0' + data).hexdigest()


def download(url, limit, raw=False):
    request = urllib.request.Request(url, headers={
        'User-Agent': 'Ultimate-Remote-Cardputer-Zero',
        'Accept': 'application/vnd.github.raw+json' if raw else 'application/vnd.github+json',
        'X-GitHub-Api-Version': '2022-11-28',
    })
    try:
        with urllib.request.urlopen(request, timeout=15) as response:
            data = response.read(limit + 1)
        if len(data) > limit:
            raise CatalogError('Download is too large')
        return data
    except urllib.error.HTTPError as error:
        if error.code in (403, 429):
            raise CatalogError('GitHub limit reached. Try again later') from error
        if error.code == 404:
            raise CatalogError('Remote not found. Refresh the catalog') from error
        raise CatalogError('GitHub is unavailable. Try again later') from error
    except (urllib.error.URLError, TimeoutError, OSError) as error:
        raise CatalogError('Cannot connect. Check Wi-Fi and retry') from error


def atomic_write(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(dir=path.parent, prefix='.download-', delete=False) as output:
            temporary = Path(output.name)
            output.write(data)
        os.replace(temporary, path)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


class Catalog:
    def __init__(self, root=None, fetch=download):
        cache_home = Path(os.environ.get('XDG_CACHE_HOME') or Path.home() / '.cache')
        self.root = Path(root or os.environ.get('ULTIMATE_REMOTE_IRDB_CACHE') or
                         cache_home / 'ultimate-remote' / 'flipper-irdb')
        self.fetch = fetch
        self.notice = 'Saved catalog'

    def validate(self, data):
        if not isinstance(data, dict) or data.get('version') != 1 or not SHA.fullmatch(str(data.get('revision', ''))):
            raise CatalogError('Invalid catalog')
        files = data.get('files')
        if not isinstance(files, dict) or not files or len(files) > 100000:
            raise CatalogError('Invalid catalog')
        for path, sha in files.items():
            if not safe_path(path) or not path.lower().endswith('.ir') or not isinstance(sha, str) or not SHA.fullmatch(sha):
                raise CatalogError('Invalid catalog entry')
        return data

    def load(self):
        try:
            data = (self.root / 'index.json').read_bytes()
            if len(data) > MAX_INDEX:
                return None
            return self.validate(json.loads(data))
        except (OSError, ValueError, CatalogError):
            return None

    def index(self, refresh=False):
        cached = self.load()
        if cached and not refresh:
            return cached
        try:
            tree = json.loads(self.fetch(API + '/git/trees/main?recursive=1', MAX_INDEX))
            if not isinstance(tree, dict) or tree.get('truncated') is not False or not isinstance(tree.get('tree'), list):
                raise CatalogError('Incomplete catalog. Retry later')
            files = {item['path']: item['sha'] for item in tree['tree']
                     if isinstance(item, dict) and item.get('type') == 'blob' and
                     item.get('mode') in ('100644', '100755') and
                     safe_path(item.get('path')) and item['path'].lower().endswith('.ir')}
            result = self.validate({'version': 1, 'revision': tree.get('sha'), 'files': files})
            atomic_write(self.root / 'index.json', json.dumps(result, ensure_ascii=True).encode())
            self.notice = 'Catalog updated'
            return result
        except (CatalogError, ValueError, KeyError, OSError) as error:
            if cached:
                self.notice = str(error) if isinstance(error, CatalogError) else 'Cannot update catalog'
                self.notice += '. Using saved catalog'
                return cached
            if isinstance(error, CatalogError):
                raise
            raise CatalogError('Cannot load catalog. Check connection and storage') from error

    def cached_blob(self, sha):
        path = self.root / 'remotes' / (sha + '.ir')
        try:
            if path.stat().st_size <= MAX_REMOTE:
                data = path.read_bytes()
                if blob_sha(data) == sha:
                    return data
        except OSError:
            pass
        return None

    def remote(self, path):
        index = self.load()
        if not safe_path(path) or not index or path not in index['files']:
            raise CatalogError('Remote not in catalog. Refresh first')
        sha = index['files'][path]
        data = self.cached_blob(sha)
        if data is None:
            data = self.fetch(API + '/git/blobs/' + sha, MAX_REMOTE, raw=True)
            if blob_sha(data) != sha:
                raise CatalogError('Download verification failed. Retry')
            self.validate_remote(data)
            atomic_write(self.root / 'remotes' / (sha + '.ir'), data)
        self.validate_remote(data)
        return data

    @staticmethod
    def validate_remote(data):
        try:
            text = data.decode('utf-8-sig')
        except UnicodeDecodeError as error:
            raise CatalogError('Invalid IR file encoding') from error
        lines = [line.strip() for line in text.splitlines() if line.strip() and not line.lstrip().startswith('#')]
        if len(lines) < 3 or lines[:2] != ['Filetype: IR signals file', 'Version: 1'] or '\0' in text:
            raise CatalogError('Invalid Flipper IR file')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=('catalog', 'fetch'))
    parser.add_argument('path', nargs='?', default='')
    parser.add_argument('--refresh', action='store_true')
    args = parser.parse_args()
    catalog = Catalog()
    try:
        if args.action == 'fetch':
            sys.stdout.buffer.write(catalog.remote(args.path))
        else:
            data = catalog.index(args.refresh)
            print('OK\t' + catalog.notice)
            for path, sha in sorted(data['files'].items(), key=lambda item: item[0].casefold()):
                saved = (catalog.root / 'remotes' / (sha + '.ir')).is_file()
                print(path + '\t' + ('Saved' if saved else 'Fetch'))
    except (CatalogError, OSError) as error:
        message = str(error) if isinstance(error, CatalogError) else 'Cannot write cache. Check storage space'
        print('ERROR\t' + message, file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
