// Allocators -*- C++ -*-

// Copyright (C) 2001-2015 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 * Copyright (c) 1996-1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file ext/pool_allocator.h
 *  This file is a GNU extension to the Standard C++ Library.
 */

#ifndef _POOL_ALLOCATOR_H
#define _POOL_ALLOCATOR_H 1

#include <bits/c++config.h>
#include <cstdlib>
#include <new>
#include <bits/functexcept.h>
#include <ext/atomicity.h>
#include <ext/concurrence.h>
#include <bits/move.h>
#if __cplusplus >= 201103L
#include <type_traits>
#endif

#ifdef STL_DEBUG
#include <glog/logging.h>
#endif

namespace __gnu_cxx _GLIBCXX_VISIBILITY(default)
{
  _GLIBCXX_BEGIN_NAMESPACE_VERSION

    using std::size_t;
  using std::ptrdiff_t;

  /**
   *  @brief  Base class for __pool_alloc.
   *
   *  Uses various allocators to fulfill underlying requests (and makes as
   *  few requests as possible when in default high-speed pool mode).
   *
   *  Important implementation properties:
   *  0. If globally mandated, then allocate objects from new
   *  1. If the clients request an object of size > _S_max_bytes, the resulting
   *     object will be obtained directly from new
   *  2. In all other cases, we allocate an object of size exactly
   *     _S_round_up(requested_size).  Thus the client has enough size
   *     information that we can return the object to the proper free list
   *     without permanently losing part of the object.
   */
  class __pool_alloc_base
  {
    protected:

      enum { _S_align = 8 }; // 内存池分出去最小的内存是8字节
      enum { _S_max_bytes = 128 }; // 内存池分出去最大的内存是128字节
      // 从8,16，24到128需要16个free_list来进行管理
      enum { _S_free_list_size = (size_t)_S_max_bytes / (size_t)_S_align };

      union _Obj
      {
        union _Obj* _M_free_list_link;
        char        _M_client_data[1];    // The client sees this.
      };

      static _Obj* volatile         _S_free_list[_S_free_list_size];

      // Chunk allocation state.
      static char*                  _S_start_free; // 内存池开始的位置
      static char*                  _S_end_free;  // 内存池结束的位置
      static size_t                 _S_heap_size;  // 统计使用了heap的内存大小

      size_t
        _M_round_up(size_t __bytes) // 将输入的字节大小扩展成8字节的倍数
        { return ((__bytes + (size_t)_S_align - 1) & ~((size_t)_S_align - 1)); }

      // 根据需要内存的大小返回相对应的自由链表
      _GLIBCXX_CONST _Obj* volatile*
        _M_get_free_list(size_t __bytes) throw ();

      __mutex&
        _M_get_mutex() throw ();

      // Returns an object of size __n, and optionally adds to size __n
      // free list.
      void*
        _M_refill(size_t __n);

      // Allocates a chunk for nobjs of size size.  nobjs may be reduced
      // if it is inconvenient to allocate the requested number.
      char*
        _M_allocate_chunk(size_t __n, int& __nobjs);
  };


  /**
   * @brief  Allocator using a memory pool with a single lock.
   * @ingroup allocators
   */
  template<typename _Tp>
    class __pool_alloc : private __pool_alloc_base
  {
    private:
      static _Atomic_word	    _S_force_new;

    public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;
      typedef _Tp        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef __pool_alloc<_Tp1> other; };

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 2103. propagate_on_container_move_assignment
      typedef std::true_type propagate_on_container_move_assignment;
#endif
      // __pool_alloc的构造和析构和一般的分配器一样
      __pool_alloc() _GLIBCXX_USE_NOEXCEPT { }

      __pool_alloc(const __pool_alloc&) _GLIBCXX_USE_NOEXCEPT { }

      template<typename _Tp1>
        __pool_alloc(const __pool_alloc<_Tp1>&) _GLIBCXX_USE_NOEXCEPT { }

      ~__pool_alloc() _GLIBCXX_USE_NOEXCEPT { }

      pointer
        address(reference __x) const _GLIBCXX_NOEXCEPT
        { return std::__addressof(__x); }

      const_pointer
        address(const_reference __x) const _GLIBCXX_NOEXCEPT
        { return std::__addressof(__x); }

      size_type
        max_size() const _GLIBCXX_USE_NOEXCEPT 
        { return size_t(-1) / sizeof(_Tp); }

#if __cplusplus >= 201103L
      template<typename _Up, typename... _Args>
        void
        construct(_Up* __p, _Args&&... __args)
        { ::new((void *)__p) _Up(std::forward<_Args>(__args)...); }

      template<typename _Up>
        void 
        destroy(_Up* __p) { __p->~_Up(); }
#else
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 402. wrong new expression in [some_] allocator::construct
      void 
        construct(pointer __p, const _Tp& __val) 
        { ::new((void *)__p) _Tp(__val); }

      void 
        destroy(pointer __p) { __p->~_Tp(); }
#endif

      pointer
        allocate(size_type __n, const void* = 0);

      void
        deallocate(pointer __p, size_type __n);      
  };

  template<typename _Tp>
    inline bool
    operator==(const __pool_alloc<_Tp>&, const __pool_alloc<_Tp>&)
    { return true; }

  template<typename _Tp>
    inline bool
    operator!=(const __pool_alloc<_Tp>&, const __pool_alloc<_Tp>&)
    { return false; }

  template<typename _Tp>
    _Atomic_word
    __pool_alloc<_Tp>::_S_force_new; 
  // 这个_S_force_new是_Atomic_word类型，是一种原子类型，具体说明可以参考
  // https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_concurrency.html

  template<typename _Tp>
    _Tp*
    __pool_alloc<_Tp>::allocate(size_type __n, const void*)
    {
      pointer __ret = 0;
      // 这个__builtin_expect是gcc的一个函数，可以更好的利用CPU的流水线
      // 参考资料：https://stackoverflow.com/questions/7346929/what-is-the-advantage-of-gccs-builtin-expect-in-if-else-statements
      if (__builtin_expect(__n != 0, true))
      {
        if (__n > this->max_size())
          std::__throw_bad_alloc();

        // If there is a race through here, assume answer from getenv
        // will resolve in same direction.  Inspired by techniques
        // to efficiently support threading found in basic_string.h.
        if (_S_force_new == 0)
        {
          // 如果设定了环境变量GLIBCXX_FORCE_NEW，则使用全局operator new来直接
          // 分配内存，如果没有设置的话会使用内存池进行内存分配
          if (std::getenv("GLIBCXX_FORCE_NEW"))
            __atomic_add_dispatch(&_S_force_new, 1);
          else
            __atomic_add_dispatch(&_S_force_new, -1);
        }
#ifdef STL_DEBUG
        if(_S_force_new > 0){
          LOG(INFO) << "force use new allocator";
        }
        else{
          LOG(INFO) << "use memory pool allocator";
        }
#endif
        const size_t __bytes = __n * sizeof(_Tp);	      
        if (__bytes > size_t(_S_max_bytes) || _S_force_new > 0)
          // 可以看到如果字节数过大，或者加载了全局环境变量，则直接调用operator new函数
          __ret = static_cast<_Tp*>(::operator new(__bytes));
        else
        {
          _Obj* volatile* __free_list = _M_get_free_list(__bytes);

          __scoped_lock sentry(_M_get_mutex()); // 加了锁，这样多线程分配内存的时候就不会打架
          _Obj* __restrict__ __result = *__free_list;
          if (__builtin_expect(__result == 0, 0))
            // 如果遇到分配出来的内存块指针是nullptr，说明需要重新向系统申请内存了，则会调用
            // _M_fill函数申请内存了
            __ret = static_cast<_Tp*>(_M_refill(_M_round_up(__bytes)));
          else
          {
            // 讲自由链表的头部内存取出来供给用户使用，然后头部链表向后面移动一位
            *__free_list = __result->_M_free_list_link;
            __ret = reinterpret_cast<_Tp*>(__result);
          }
          if (__ret == 0)
            std::__throw_bad_alloc();
        }
      }
      return __ret;
    }

  template<typename _Tp>
    void
    __pool_alloc<_Tp>::deallocate(pointer __p, size_type __n)
    {
      if (__builtin_expect(__n != 0 && __p != 0, true))
      {
        const size_t __bytes = __n * sizeof(_Tp);
        if (__bytes > static_cast<size_t>(_S_max_bytes) || _S_force_new > 0)
          // 这个是直接将内存还给底层glibc库了
          ::operator delete(__p);
        else
        {
          // 如果大小在8到128字节的内存会丢还给自由链表来进行管理，从这里我们也可以看出一个问题
          // 就是pool_alloc会慢慢的占据大量的内存，而这些内存没有归还给底层glibc库，这个是有一定的
          // 问题的，因而这也是pool_alloc不在作为标准分配器的一个原因吧
          _Obj* volatile* __free_list = _M_get_free_list(__bytes);
          _Obj* __q = reinterpret_cast<_Obj*>(__p);

          __scoped_lock sentry(_M_get_mutex());
          __q ->_M_free_list_link = *__free_list;
          *__free_list = __q;
        }
      }
    }

  _GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif
