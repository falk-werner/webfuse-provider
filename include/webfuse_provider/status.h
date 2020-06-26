////////////////////////////////////////////////////////////////////////////////
/// \file status.h
/// \brief Generic status code.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_STATUS_H
#define WFP_STATUS_H

#define WFP_GOOD 0   ///< Positive status code.
#define WFP_BAD  1   ///< Generic negative status code.

#define WFP_BAD_NOTIMPLEMENTED 2     ///< The called function is not implemented (yet).
#define WFP_BAD_TIMEOUT        3     ///< A timeout occured.
#define WFP_BAD_BUSY           4     ///< Resource is busy, try again later.
#define WFP_BAD_FORMAT         5     ///< Invalid format.

#define WFP_BAD_NOENTRY 101          ///< Entry not found.
#define WFP_BAD_ACCESS_DENIED 102    ///< Access is denied.

/// Status code.
typedef int wfp_status;

#endif
