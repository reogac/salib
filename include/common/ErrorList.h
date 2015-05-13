/**
 @file ErrorList.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  ErrorList_INC
#define  ErrorList_INC

#include <memory>
#include <vector>

#include "ErrorMessage.h"

BIO_NAMESPACE_BEGIN

#define LOG_ERRORS(errors) Logger::instance() << errors;

class ErrorList
{
  public:
    ErrorList();
    ErrorList(ErrorList&& other);
    ErrorList(const ErrorList& other) = delete;
    int getNumMessages() const;

    int getNumErrors() const;
    int getNumFatals() const;
    int getNumWarnings() const;

    const ErrorMessage& getError(const int index) const;

    void add(std::unique_ptr<ErrorMessage>&& message) noexcept;
    void add(ErrorMessage*&& message) noexcept;
    void clear();
  protected:
    int getNumErrors( const ErrorLevel_t level) const;
    std::vector<std::unique_ptr<ErrorMessage> > m_Errors;
  private:
    friend Logger& operator<<(Logger& logger, const ErrorList& errors);
    virtual void write(Logger& logger) const;

    int m_NumErrors;
    int m_NumFatals;
    bool m_Valid;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef ErrorList_INC  ----- */
