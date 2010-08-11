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

#include "aarrayts.h"

HRESULT AArrayTextService::OnKeyDown123( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten )
{
	* pfEaten = FALSE;

    int vKey = static_cast<int>( wParam ); // 0x60 = 96 -> 48
	int asciiKey = vKey - 48;

	if( state == ST_INIT )
	{
		cwin.composition = L"";
		cwin.candidates = L"";

		if( vKey >= 0x60 && vKey <=0x69 )
		{
			* pfEaten = TRUE;

			if( lexicon->put( asciiKey ) == AArrayLex::ST_VALID )
			{
				state = ST_COMPOSITE;
				cwin.composition += asciiKey;
				cwin.candidates += lexicon->getCandidateString( asciiKey );
			}
		}

		else
			* pfEaten = FALSE;
	}
	else if( state == ST_COMPOSITE )
	{
		cwin.candidates = L"";

		if( vKey >= 0x60 && vKey <=0x69 )
		{
			* pfEaten = TRUE;

			if( lexicon->put( asciiKey ) == AArrayLex::ST_VALID )
			{
				state = ST_COMPOSITE;
				cwin.composition += asciiKey;
				cwin.candidates += lexicon->getCandidateString( asciiKey );
			}
			else
			{
				state = ST_INIT;
				cwin.candidates = L"錯誤";
				cwin.composition = L"";
				lexicon->put( 0 );
			}
		}

		else if( vKey == VK_SPACE )
		{
			* pfEaten = TRUE;
			state = ST_INIT;

			// commit to edit sessionn
			vector<wchar_t> canvec =lexicon->getCandidates();
			if( canvec.at( 0 ) != 0 )
			{
				cwin.composition = L"";
				cwin.candidates = L"";
				commit( pContext, canvec.at( 0 ) );
			}
			else // no word
			{
				cwin.composition = L"";
				cwin.candidates = L"無此字";
			}
			lexicon->put( 0 ); //reset
		}

		else if( vKey == VK_BACK )
		{
			if( lexicon->backable )
			{
    	        * pfEaten = TRUE;
				lexicon->back();
				cwin.composition.erase( cwin.composition.end() - 2, cwin.composition.end() );

				if( cwin.composition.size() == 0 )
				{
					cwin.candidates = L"";
                    state = ST_INIT;
				}
				else
					cwin.candidates += lexicon->getCandidateString();
			}
			else
				* pfEaten = FALSE;
		}

		else if( vKey == VK_ESCAPE )
		{
			cwin.composition = L"";
            cwin.candidates = L"";

            lexicon->put( 0 ); //reset

            * pfEaten = TRUE;
            state = ST_INIT;
		}

        else if( vKey == VK_RETURN )
		{
    		* pfEaten = TRUE;
			state = ST_SELECT;
            cwin.candidates = lexicon->getCandidateString();
            cwin.composition += L"<";
		}
	}
	else if( state == ST_SELECT )
	{
		if( vKey >= 0x60 && vKey <=0x69 )
		{
			int index = vKey - 0x61;
			if( index == -1 )
				index = 9;

            vector<wchar_t> canvec = lexicon->getCandidates();

			if( index < canvec.size() )
			{
				state = ST_INIT;
//        	    cwin.composition = L"";
//				cwin.candidates = L"";

            	// commit to edit sessionn
				commit( pContext, canvec.at( index ) );
			}

       	    cwin.composition = L"";
            cwin.candidates = L"";
            * pfEaten = TRUE;
			lexicon->put( 0 ); //reset
		}
		else if( vKey == VK_RETURN )
		{
			* pfEaten = TRUE;
			state = ST_INIT;

			// commit to edit sessionn
			vector<wchar_t> canvec =lexicon->getCandidates();
			if( canvec.at( 0 ) != 0 )
			{
				cwin.composition = L"";
				cwin.candidates = L"";
				commit( pContext, canvec.at( 0 ) );
			}
			else // no word
			{
				cwin.composition = L"";
				cwin.candidates = L"無此字";
			}
			lexicon->put( 0 ); //reset
		}

		else if( vKey == VK_ESCAPE )
		{
			cwin.composition = L"";
            cwin.candidates = L"";

            lexicon->put( 0 ); //reset

            * pfEaten = TRUE;
            state = ST_INIT;
		}
	}

	cwin.redraw();
	return S_OK;
}