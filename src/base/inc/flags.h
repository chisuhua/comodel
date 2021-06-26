#pragma once

template <typename T>
class Flags
{
  private:
    T _flags;

  public:
    typedef T Type;
    Flags() : _flags(0) {}
    Flags(Type flags) : _flags(flags) {}

    operator const Type() const { return _flags; }

    template <typename U>
    const Flags<T> &
    operator=(const Flags<U> &flags)
    {
        _flags = flags._flags;
        return *this;
    }

    const Flags<T> &
    operator=(T flags)
    {
        _flags = flags;
        return *this;
    }

    bool isSet() const { return _flags; }
    bool isSet(Type flags) const { return (_flags & flags); }
    bool allSet() const { return !(~_flags); }
    bool allSet(Type flags) const { return (_flags & flags) == flags; }
    bool noneSet() const { return _flags == 0; }
    bool noneSet(Type flags) const { return (_flags & flags) == 0; }
    void clear() { _flags = 0; }
    void clear(Type flags) { _flags &= ~flags; }
    void set(Type flags) { _flags |= flags; }
    void set(Type f, bool val) { _flags = (_flags & ~f) | (val ? f : 0); }
    void
    update(Type flags, Type mask)
    {
        _flags = (_flags & ~mask) | (flags & mask);
    }
};

