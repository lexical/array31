#include "aarrayts.h"

HRESULT AArrayTextService::OnKeyDown30( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten )
{
	int vKey = static_cast<int>( wParam );

	if( state == ST_INIT )
	{
		//TRACEX( "ST_INIT\n" );
		cwin.composition = L"";
		cwin.candidates = L"";

		if( ( vKey >= 0x41 && vKey <= 0x5a ) || vKey == 0xba || vKey == 0xbc || vKey == 0xbe || vKey == 0xbf )
		{
			* pfEaten = TRUE;

			if( lexicon->put( getAsciiFromVkey( vKey ) ) == AArrayLex::ST_VALID )
			{
				if( vKey == 0x57 )
                    stateSymbolReady = true;

        		state = ST_COMPOSITE;
				cwin.composition += getStrokeFromVkey( vKey );
				cwin.candidates += lexicon->getCandidateString( getAsciiFromVkey( vKey ) );
				simpleCodeKey = vKey;
			}
			else
			{
				state = ST_INIT;
				cwin.candidates = L"錯誤";
				cwin.composition = L"";
				lexicon->put( 0 );
			}
			cwin.redraw();
		}
		else if ( vKey == 0xc0 /*`~*/ ) // SYMBOL
		{
            if( lexicon->put( getAsciiFromVkey( vKey ) ) == AArrayLex::ST_VALID )
			{
			    * pfEaten = TRUE;
				state = ST_SYMBOL;
				cwin.composition += getStrokeFromVkey( vKey );
				cwin.redraw();
			}
		}
		else
			* pfEaten = FALSE;
	}
	else if( state == ST_COMPOSITE )
	{
		cwin.candidates = L"";

		if( ( vKey >= 0x41 && vKey <= 0x5a ) || vKey == 0xba || vKey == 0xbc || vKey == 0xbe || vKey == 0xbf )
		{
			* pfEaten = TRUE;
			stateSymbolReady = false;
            simpleCodeKey = 0;

			if( lexicon->put( getAsciiFromVkey( vKey ) ) == AArrayLex::ST_VALID )
			{
        		state = ST_COMPOSITE;
                cwin.composition += getStrokeFromVkey( vKey );
                cwin.candidates += lexicon->getCandidateString();
			}
			else
			{
				state = ST_INIT;
				cwin.candidates = L"無此字";
				cwin.composition = L"";
				lexicon->put( 0 );
			}

			cwin.redraw();
		}
		else if( vKey == VK_SPACE )
		{
			* pfEaten = TRUE;
			stateSymbolReady = false;
			state = ST_INIT;

			// commit to edit sessionn
			vector<wchar_t> canvec =lexicon->getCandidates();
			if( canvec.at( 0 ) != 0 )
			{
				cwin.composition = L"";
				cwin.candidates = L"";
				cwin.redraw();

				commit( pContext, canvec.at( 0 ) );
			}
			else // no word
			{
				cwin.composition = L"";
				cwin.candidates = L"無此字";
				cwin.redraw();

			}
			lexicon->put( 0 ); //reset
            simpleCodeKey = 0;
		}
		else if( vKey >= 0x30 && vKey <= 0x39 ) // number
		{
			int index( 0 );

			if( vKey == 0x30 )
				index = 9;
			else
				index = vKey - 0x31;

			if( false && stateSymbolReady )
			{
				// show symbol candidates
				// goto ST_SYMBOL
			}
			else
			{
	            vector<wchar_t> canvec = lexicon->getCandidates( getAsciiFromVkey( simpleCodeKey ) );

				if( index < canvec.size() )
				{
		            * pfEaten = TRUE;
					state = ST_INIT;
	        	    cwin.composition = L"";
					cwin.candidates = L"";
	            	cwin.redraw();

	            	// commit to edit sessionn
					commit( pContext, canvec.at( index ) );
					lexicon->put( 0 ); //reset
                    simpleCodeKey = 0;
				}
				else
					*pfEaten = FALSE;
			}
		}
		else if( vKey == VK_BACK )
		{
            stateSymbolReady = false;
            simpleCodeKey = 0;
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

				cwin.redraw();
			}
			//else
			//	* pfEaten = FALSE;
		}
		else if( vKey == VK_ESCAPE )
		{
            stateSymbolReady = false;
			simpleCodeKey = 0;
			cwin.composition = L"";
            cwin.candidates = L"";
            cwin.redraw();

            lexicon->put( 0 ); //reset

            * pfEaten = TRUE;
            state = ST_INIT;
		}
		else
		{
            stateSymbolReady = false;
            * pfEaten = FALSE;
		}
	}
    else if( state == ST_SYMBOL )
	{
		if( vKey == VK_ESCAPE )
		{
			cwin.composition = L"";
            cwin.candidates = L"";
            cwin.redraw();

            //lexicon->put( 0 ); //reset

            * pfEaten = TRUE;
            state = ST_INIT;
		}
		else if( vKey == VK_OEM_3 ) // `~
		{
            * pfEaten = TRUE;
			state = ST_INIT;
    	    cwin.composition = L"";
			cwin.candidates = L"";
        	cwin.redraw();

        	// commit to edit sessionn
			commit( pContext, L'`' );
		}
		if( ( vKey >= 0x41 && vKey <= 0x5a ) || vKey == 0xba || vKey == 0xbc || vKey == 0xbe || vKey == 0xbf )
		{
			* pfEaten = TRUE;
			stateSymbolReady = false;
            simpleCodeKey = 0;

			if( lexicon->put( getAsciiFromVkey( vKey ) ) == AArrayLex::ST_VALID )
			{
        		state = ST_COMPOSITE;
                cwin.composition += getStrokeFromVkey( vKey );
                cwin.candidates += lexicon->getCandidateString();
			}
			else
			{
				state = ST_INIT;
				cwin.candidates = L"無此字";
				cwin.composition = L"";
				lexicon->put( 0 );
			}

			cwin.redraw();
		}
		else
		{
            * pfEaten = FALSE;
		}
	}
	else
	{
		* pfEaten = FALSE;
	}

	// move candidate window
/*    ITfContextView * pContextView;
	if( pContext->GetActiveView( & pContextView ) != S_OK ) TRACEX( "GetActiveView failed\n" );
	RECT contextRect;
	if( pContextView->GetScreenExt( & contextRect ) != S_OK ) TRACEX( "GetScreenExt failed\n" );
	cwin.move( contextRect.left, contextRect.top );
*/

	return S_OK;
}
