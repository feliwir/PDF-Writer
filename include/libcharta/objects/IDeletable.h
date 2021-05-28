#pragma once

namespace charta
{
class IDeletable
{
  public:
    virtual void DeleteMe() = 0;
};
} // namespace charta