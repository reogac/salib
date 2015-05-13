/**
 @file namespace.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  namespace_INC
#define  namespace_INC

#ifdef __cplusplus

#define BIO_NAMESPACE reo
#define BIO_NAMESPACE_BEGIN namespace BIO_NAMESPACE {
#define BIO_NAMESPACE_END }

#define BIO_NAMESPACE_USE using namespace BIO_NAMESPACE;

//namespace BIO_NAMESPACE {}
#endif //__cplusplus

#endif   /* ----- #ifndef namespace_INC  ----- */
