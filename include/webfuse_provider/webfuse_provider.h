////////////////////////////////////////////////////////////////////////////////
/// \file webfuse_provider.h
/// \brief Convenience header to include all functionality of libfuse_provider.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_PROVIDER_H
#define WF_PROVIDER_H

#include <webfuse_provider/status.h>
#include <webfuse_provider/protocol_names.h>

#include <webfuse_provider/api.h>
#include <webfuse_provider/client.h>
#include <webfuse_provider/client_config.h>
#include <webfuse_provider/client_protocol.h>
#include <webfuse_provider/dirbuffer.h>
#include <webfuse_provider/credentials.h>

#include <webfuse_provider/operation/error.h>
#include <webfuse_provider/operation/lookup.h>
#include <webfuse_provider/operation/getattr.h>
#include <webfuse_provider/operation/readdir.h>
#include <webfuse_provider/operation/open.h>
#include <webfuse_provider/operation/close.h>
#include <webfuse_provider/operation/read.h>

#endif
