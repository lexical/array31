/*
 *   Copyright 2010 Keng-Yu Lin <kengyu@lexical.tw>
 *
 *   This file is part of Array31.
 *
 *   Array31 is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Array31 is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Array31.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACLASSFACTORY_H
#define ACLASSFACTORY_H

#include <rpc.h>
#include "aarrayts.h"

extern AArrayTextService * gArrayTS;

class AClassFactory:public IClassFactory
{
public:
	AClassFactory();
	virtual ~AClassFactory();

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, void ** );
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	//IClassFactory
	virtual HRESULT STDMETHODCALLTYPE CreateInstance( IUnknown * pUnkOuter, REFIID riid, void ** ppvOjbect );
	virtual HRESULT STDMETHODCALLTYPE LockServer( BOOL fLock );

	static LONG m_cObj;
	static LONG m_cLock;

protected:
	ULONG m_cRef;
};


#endif