#pragma once
namespace Events {
	//--------------------------------------------------------------------------------
	class CBaseEventArgs {
	public:
		virtual ~CBaseEventArgs();
	};
	//--------------------------------------------------------------------------------
	template<typename T>
	class CGeneralEventArgs : public CBaseEventArgs {
		T m_Data;
		CGeneralEventArgs();
	public:
		CGeneralEventArgs(const T data);
		const T& GetData() const;
	};
	//--------------------------------------------------------------------------------
	template<typename T, typename T2>
	class CGeneralEventArgs2 : public CBaseEventArgs {
		T m_Data;
		T2 m_Data2;
		CGeneralEventArgs2();
	public:
		CGeneralEventArgs2(const T data, const T2 data2);
		const T& GetData() const;
		const T2& GetData2() const;
	};
	//--------------------------------------------------------------------------------
	inline CBaseEventArgs::~CBaseEventArgs() {}
	//--------------------------------------------------------------------------------
	template<typename T> inline CGeneralEventArgs<T>::CGeneralEventArgs() {}
	//--------------------------------------------------------------------------------
	template<typename T> inline CGeneralEventArgs<T>::CGeneralEventArgs(const T data) { m_Data = data; }
	//--------------------------------------------------------------------------------
	template<typename T> inline const T& CGeneralEventArgs<T>::GetData() const{ return m_Data; }
	//--------------------------------------------------------------------------------
	template<typename T, typename T2> inline CGeneralEventArgs2<T, T2>::CGeneralEventArgs2() {}
	//--------------------------------------------------------------------------------
	template<typename T, typename T2> inline CGeneralEventArgs2<T, T2>::CGeneralEventArgs2(const T data, const T2 data2) { m_Data = data; m_Data2 = data2; }
	//--------------------------------------------------------------------------------
	template<typename T, typename T2> inline const T& CGeneralEventArgs2<T, T2>::GetData() const { return m_Data; }
	//--------------------------------------------------------------------------------
	template<typename T, typename T2> inline const T2& CGeneralEventArgs2<T, T2>::GetData2() const { return m_Data2; }
	//--------------------------------------------------------------------------------
};