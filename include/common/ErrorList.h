/**
 @file ErrorList.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include <memory>
#include <vector>

#include "ErrorMessage.h"

BIO_NAMESPACE_BEGIN

#define LOG_ERRORS(errors) Logger::instance() << errors;

class ErrorList
{
  public:
    ErrorList();
    virtual ~ErrorList();

    int getNumMessages() const;

    int getNumErrors() const;
    int getNumFatals() const;
    int getNumWarnings() const;

    const ErrorMessage& getError(const int index) const;

    void add(ErrorMessage*&& message);
    void add(std::unique_ptr<ErrorMessage> message);
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

