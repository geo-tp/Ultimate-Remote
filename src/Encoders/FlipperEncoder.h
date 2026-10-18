#pragma once
#include <Models/FileRemoteCommand.h>
#include <Models/FlipperWaveform.h>
namespace encoders {
models::FlipperWaveform encodeFlipperCommand(const models::FileRemoteCommand& command, bool toggle);
}
