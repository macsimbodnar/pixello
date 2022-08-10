#pragma once
#include <exception>
#include <memory>
#include <string>

/*******************************************************************************
 * EXCEPTIONS
 ******************************************************************************/
class pixello_exception : public std::runtime_error
{
public:
  pixello_exception(std::string msg) : std::runtime_error(std::move(msg)) {}
};

class init_exception : public pixello_exception
{
public:
  init_exception(std::string msg) : pixello_exception(std::move(msg)) {}
};

class load_exceptions : public pixello_exception
{
public:
  load_exceptions(std::string msg) : pixello_exception(std::move(msg)) {}
};