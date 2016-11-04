#pragma once
//--------------------------------------------------------------------------------
//Taken from http://www.codeproject.com/Articles/18886/A-new-way-to-implement-Delegate-in-C
//The files have been abstracted and updated to to use both Rvalues and varaidic templates
//--------------------------------------------------------------------------------
#include "rootDelegate.h"
//--------------------------------------------------------------------------------
#pragma region CALLING_CONVENTION
//--------------------------------------------------------------------------------
template<typename _object_type, typename returnValue, typename ...Parameters>
struct thiscall_convention {
	typedef _object_type object_type;
	typedef returnValue(_object_type::*method)(Parameters...);
	typedef returnValue(_object_type::*const_method)(Parameters...) const;
	typedef thiscall_convention<_never_exist_class_, returnValue, Parameters...> rebind;
};
//--------------------------------------------------------------------------------
template<typename _object_type, typename returnValue, typename ...Parameters>
struct cdecl_convention {
	typedef _object_type object_type;
	typedef returnValue(__cdecl _object_type::*method)(Parameters...);
	typedef returnValue(__cdecl _object_type::*const_method)(Parameters...) const;
	typedef cdecl_convention<_never_exist_class_, returnValue, Parameters...> rebind;
};
//--------------------------------------------------------------------------------
template<typename _object_type, typename returnValue, typename ...Parameters>
struct stdcall_convention {
	typedef _object_type object_type;
	typedef returnValue(__stdcall _object_type::*method)(Parameters...);
	typedef returnValue(__stdcall _object_type::*const_method)(Parameters...) const;
	typedef stdcall_convention<_never_exist_class_, returnValue, Parameters...> rebind;
};
//--------------------------------------------------------------------------------
template<typename _object_type, typename returnValue, typename ...Parameters>
struct fastcall_convention {
	typedef _object_type object_type;
	typedef returnValue(__fastcall _object_type::*method)(Parameters...);
	typedef returnValue(__fastcall _object_type::*const_method)(Parameters...) const;
	typedef fastcall_convention<_never_exist_class_, returnValue, Parameters...> rebind;
};
//--------------------------------------------------------------------------------
#pragma endregion // Used to abstract how the actual function is called
#pragma region FUNCTION_CALL
//--------------------------------------------------------------------------------
// R != void
template<typename R> struct relax_if_void  {
	template<typename base_class, typename TFunction, typename ...Parameters>
	struct function_strategy : public base_class {
		virtual R operator() (Parameters...inputs) const {
			return (*(*(TFunction*)(&m_fn)))(std::forward<Parameters>(inputs)...);
		}
	};
	template<typename base_class, typename calling_convention, typename ...Parameters>
	struct method_strategy : public base_class {
		virtual R operator() (Parameters...inputs) const {
			return (m_object_ptr->*(*(typename calling_convention::rebind::method*)(&m_fn)))(std::forward<Parameters>(inputs)...);
		}
	};
};
//--------------------------------------------------------------------------------
// R == void
template<> struct relax_if_void<void> {
	template<typename base_class, typename TFunction, typename ...Parameters>
	struct function_strategy : public base_class {
		virtual void operator() (Parameters...inputs) const {
			(*(*(TFunction*)(&m_fn)))(std::forward<Parameters>(inputs)...);
		}
	};
	template<typename base_class, typename calling_convention, typename ...Parameters>
	struct method_strategy : public base_class {
		virtual void operator() (Parameters...inputs) const {
			(m_object_ptr->*(*(typename calling_convention::rebind::method*)(&m_fn)))(std::forward<Parameters>(inputs)...);
		}
	};
};
//--------------------------------------------------------------------------------
#pragma endregion // this does the actual calling of a function
//--------------------------------------------------------------------------------
template<typename returnType, typename... Parameters>
class Delegate : public delegate_root {
public:
	// default constructor
	Delegate() throw() {
		new (&m_strategy) null_strategy();
	}
	// copy constructor
	Delegate(Delegate const & _other) throw() {
		m_strategy = _other.m_strategy; // do bit-wise copy
		strategy().object_clone(); // then clone object
	}
	// move contructor
	Delegate(Delegate&& _other) {
		m_strategy = std::move(_other.m_strategy);
		strategy().object_clone();
	}
	// operator =
	Delegate& operator=(Delegate const & _other) throw() {
		delegate_root old(*this); // cleaning up automatically on return 
		new (this) Delegate(_other);
		return *this;
	}
	Delegate& operator=(Delegate&& _other) {
		delegate_root old(std::move(*this)); // cleaning up automatically on return 
		new (this) Delegate(std::forward(_other));
		return *this;
	}
	// swap between 2 delegates
	void swap(Delegate& _other) throw() {
		// all operations are just bit-wise copy
		strategy_place_holder temp(m_strategy);
		m_strategy = _other.m_strategy;
		_other.m_strategy = temp;
	}
	// reset to null
	void clear() throw() {
		strategy().object_free();
		new (&m_strategy) null_strategy();
	}
	// operator = NULL
	const Delegate& operator=(clear_type*) {
		clear();
		return *this;
	}
protected:
	struct invokable_strategy : public no_clone_strategy {
		virtual returnType operator() (Parameters...) const = 0;
	};
public:
	// Syntax 01: (*delegate)(param...)
	invokable_strategy const& operator*() const throw() {
		return static_cast<invokable_strategy const&>(strategy());
	}
	// Syntax 02: delegate(param...)
	// Note: syntax 02 might be slower than syntax 01 in some cases
	returnType operator()(Parameters...args) const {
		return static_cast<invokable_strategy const&>(strategy())(args...);
	}
	// constructor for function
	template<typename TFunction>
	Delegate(TFunction fn) {
		new (&m_strategy) function_strategy<TFunction>(fn);
	}
	// bind to function
	template<typename TFunction>
	void bind(TFunction fn) {
		strategy().object_free();
		new (&m_strategy) function_strategy<TFunction>(fn);
	}
	// constructor for method
	template<typename TMethod, typename object_type>
	Delegate(object_type* p, TMethod fn) {
		construct_method(fn, *p, clone_option<view_clone_allocator>());
	}
	// bind to method
	template<typename TMethod, typename object_type>
	void bind(object_type* p, TMethod fn) {
		strategy().object_free();
		construct_method(fn, *p, clone_option<view_clone_allocator>());
	}
	// constructor for method using clone-allocator
	template<typename TMethod, typename object_type, typename clone_info>
	Delegate(object_type* p, TMethod fn, clone_info info) {
		construct_method(fn, *p, info);
	}
	// bind to method using clone-allocator
	template<typename TMethod, typename object_type, typename clone_info>
	void bind(object_type* p, TMethod fn, clone_info info) {
		strategy().object_free();
		construct_method(fn, *p, info);
	}
protected:
	typedef clone_option<view_clone_allocator>::no_clone_strategy no_clone_strategy;

	struct null_strategy : public no_clone_strategy {
		null_strategy() {
			init_null();
		}
		virtual returnType operator() (Parameters...) const {
			throw bad_function_call();
		}
		virtual bool is_empty() const throw() {
			return true;
		}
	};
	template<typename TFunction>
	struct function_strategy : public relax_if_void<returnType>::template function_strategy<no_clone_strategy, TFunction, Parameters...> {
		function_strategy(TFunction fn) {
			init_function(reinterpret_cast<strategy_root::pointer_to_function&>(fn));
		}
	};
	template<typename clone_strategy_type, typename calling_convention>
	struct method_strategy : public relax_if_void<returnType>::template method_strategy<clone_strategy_type, calling_convention, Parameters...> {
		method_strategy(typename calling_convention::method fn, typename calling_convention::object_type & r) {
			m_object_ptr = reinterpret_cast<_never_exist_class_*>(&r);

			// translate pointer-to-member to its large form
			class _another_never_exist_class_;
			typedef typename calling_convention::object_type object_type;
			typedef	void (object_type::*small_pointer_to_method)();
			typedef void (_another_never_exist_class_::*large_pointer_to_method)();

			//This line is needed to make the class aware of a new class that it does not know about to ensure the largest pointer size
			COMPILE_TIME_ASSERT((sizeof(small_pointer_to_method) == sizeof(typename calling_convention::method)) &&
				(sizeof(large_pointer_to_method) == sizeof(strategy_root::greatest_pointer_type)));

			// Now tell the compiler about '_another_never_exist_class_"
			class _another_never_exist_class_ : public object_type {};

			reinterpret_cast<large_pointer_to_method&>(m_fn) = reinterpret_cast<const small_pointer_to_method&>(fn);
		}
	};
	template<typename clone_strategy_type, typename calling_convention>
	struct const_method_strategy : public method_strategy<clone_strategy_type, calling_convention> {
		// treat const method just as non-const method
		// ==> forward all parameter to its base class after some necessary castings
		const_method_strategy(typename calling_convention::const_method fn, typename calling_convention::object_type const& r)
			: method_strategy<clone_strategy_type, calling_convention>(reinterpret_cast<typename calling_convention::method&>(fn), const_cast<typename calling_convention::object_type&>(r)) {
		}
	};
	// construct delegate from 'thiscall' method
	template<typename base_type, typename returnType, typename ...Parameters, typename object_type, typename clone_info>
	void construct_method(returnType(base_type::*fn)(Parameters...), object_type& r, clone_info info) throw() {
		new (&m_strategy) method_strategy<typename clone_info::template strategy<object_type, base_type>, thiscall_convention<base_type, returnType, Parameters...> >(fn, r);
		if (info) {
			strategy().object_clone();
		}
	}
	// construct delegate from 'thiscall' const-method
	template<typename base_type, typename returnType, typename ...Parameters, typename object_type, typename clone_info>
	void construct_method(returnType(base_type::*fn)(Parameters...) const, object_type& r, clone_info info) throw() {
		new (&m_strategy) const_method_strategy<typename clone_info::template strategy<object_type, base_type>, thiscall_convention<base_type, returnType, Parameters...> >(fn, r);
		if (info) {
			strategy().object_clone();
		}
	}
	// construct delegate from 'cdecl' method
	template<typename base_type, typename returnType, typename ...Parameters, typename object_type, typename clone_info>
	void construct_method(returnType(__cdecl base_type::*fn)(Parameters...), object_type& r, clone_info info) throw() {
		new (&m_strategy) method_strategy<typename clone_info::template strategy<object_type, base_type>, cdecl_convention<base_type, returnType, Parameters...> >(fn, r);
		if (info) {
			strategy().object_clone();
		}
	}
	// construct delegate from 'cdecl' const-method
	template<typename base_type, typename returnType, typename ...Parameters, typename object_type, typename clone_info>
	void construct_method(returnType(__cdecl base_type::*fn)(Parameters...) const, object_type& r, clone_info info) throw() {
		new (&m_strategy) const_method_strategy<typename clone_info::template strategy<object_type, base_type>, cdecl_convention<base_type, returnType, Parameters...> >(fn, r);
		if (info) {
			strategy().object_clone();
		}
	}
	// construct delegate from 'stdcall' method
	template<typename base_type, typename returnType, typename ...Parameters, typename object_type, typename clone_info>
	void construct_method(returnType(__stdcall base_type::*fn)(Parameters...), object_type& r, clone_info info) throw() {
		new (&m_strategy) method_strategy<typename clone_info::template strategy<object_type, base_type>, stdcall_convention<base_type, returnType, Parameters...> >(fn, r);
		if (info) {
			strategy().object_clone();
		}
	}
	// construct delegate from 'stdcall' const-method
	template<typename base_type, typename returnType, typename ...Parameters, typename object_type, typename clone_info>
	void construct_method(returnType(__stdcall base_type::*fn)(Parameters...) const, object_type& r, clone_info info) throw() {
		new (&m_strategy) const_method_strategy<typename clone_info::template strategy<object_type, base_type>, stdcall_convention<base_type, returnType, Parameters...> >(fn, r);
		if (info) {
			strategy().object_clone();
		}
	}
	// construct delegate from 'fastcall' method
	template<typename base_type, typename returnType, typename ...Parameters, typename object_type, typename clone_info>
	void construct_method(returnType(__fastcall base_type::*fn)(Parameters...), object_type& r, clone_info info) throw() {
		new (&m_strategy) method_strategy<typename clone_info::template strategy<object_type, base_type>, fastcall_convention<base_type, returnType, Parameters...> >(fn, r);
		if (info) {
			strategy().object_clone();
		}
	}
	// construct delegate from 'fastcall' const-method
	template<typename base_type, typename returnType, typename ...Parameters, typename object_type, typename clone_info>
	void construct_method(returnType(__fastcall base_type::*fn)(Parameters...) const, object_type& r, clone_info info) throw() {
		new (&m_strategy) const_method_strategy<typename clone_info::template strategy<object_type, base_type>, fastcall_convention<base_type, returnType, Parameters...> >(fn, r);
		if (info) {
			strategy().object_clone();
		}
	}
};
//--------------------------------------------------------------------------------