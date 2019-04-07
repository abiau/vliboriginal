#ifndef __VS_HTTP_STATUS_H__
#define __VS_HTTP_STATUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 2XX Success. */
#define VS_HTTP_OK                        200
#define VS_HTTP_CREATED                   201
#define VS_HTTP_ACCEPTED                  202
#define VS_HTTP_NO_CONTENT                204
#define VS_HTTP_RESET_CONTENT             205
#define VS_HTTP_PARTIAL_CONTENT           206

/* 3XX Redirection. */
#define VS_HTTP_SPECIAL_RESPONSE          300
#define VS_HTTP_MOVED_PERMANENTLY         301
#define VS_HTTP_MOVED_TEMPORARILY         302
#define VS_HTTP_SEE_OTHER                 303
#define VS_HTTP_NOT_MODIFIED              304
#define VS_HTTP_TEMPORARY_REDIRECT        307

/* 4XX Client Error. */
#define VS_HTTP_BAD_REQUEST               400
#define VS_HTTP_UNAUTHORIZED              401
#define VS_HTTP_FORBIDDEN                 403
#define VS_HTTP_NOT_FOUND                 404
#define VS_HTTP_NOT_ALLOWED               405
#define VS_HTTP_REQUEST_TIME_OUT          408
#define VS_HTTP_CONFLICT                  409
#define VS_HTTP_LENGTH_REQUIRED           411
#define VS_HTTP_PRECONDITION_FAILED       412
#define VS_HTTP_REQUEST_ENTITY_TOO_LARGE  413
#define VS_HTTP_REQUEST_URI_TOO_LARGE     414
#define VS_HTTP_UNSUPPORTED_MEDIA_TYPE    415
#define VS_HTTP_RANGE_NOT_SATISFIABLE     416
#define VS_HTTP_MISDIRECTED_REQUEST       421

/* 5XX Server Error. */
#define VS_HTTP_INTERNAL_SERVER_ERROR     500
#define VS_HTTP_NOT_IMPLEMENTED           501
#define VS_HTTP_BAD_GATEWAY               502
#define VS_HTTP_SERVICE_UNAVAILABLE       503
#define VS_HTTP_GATEWAY_TIME_OUT          504
#define VS_HTTP_INSUFFICIENT_STORAGE      507

const char* HttpStatusStr (int status);

#ifdef __cplusplus
}
#endif

#endif  //__VS_HTTP_STATUS_H__

