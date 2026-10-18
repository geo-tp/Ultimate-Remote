#ifndef INFRARED_SEND_RESULT_ENUM_H
#define INFRARED_SEND_RESULT_ENUM_H

namespace enums {
// Distinguish unavailable IR protocols from failures of the actual hardware
// sender. Scanning may skip unsupported commands but must stop on TX failure.
enum class InfraredSendResultEnum { Sent, Unsupported, TransmitFailed };
}

#endif
