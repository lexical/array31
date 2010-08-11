#include "aarrayts.h"

HRESULT AArrayTextService::OnKeyDown40( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten )
{
    int vKey = static_cast<int>( wParam );

	if( state == ST_INIT )
	{
		cwin.composition = L"";
		cwin.candidates = L"";

        if( ( vKey >= 0x30 && vKey <= 0x39 ) || ( vKey >= 0x41 && vKey <= 0x5a ) || vKey == 0xba || vKey == 0xbc || vKey == 0xbe || vKey == 0xbf )
		{
			* pfEaten = TRUE;

			if( lexicon->put( getAsciiFromVkey( vKey ) ) == AArrayLex::ST_VALID )
			{
				state = ST_COMPOSITE;
				cwin.composition += getAsciiFromVkey( vKey );
				cwin.candidates += lexicon->getCandidateString( getAsciiFromVkey( vKey ) );
			}
		}
		else
			* pfEaten = FALSE;
	}
	else if( state == ST_COMPOSITE )
	{
		cwin.candidates = L"";

        if( ( vKey >= 0x30 && vKey <= 0x39 ) || ( vKey >= 0x41 && vKey <= 0x5a ) || vKey == 0xba || vKey == 0xbc || vKey == 0xbe || vKey == 0xbf )
		{
			* pfEaten = TRUE;


			if( lexicon->put( getAsciiFromVkey( vKey ) ) == AArrayLex::ST_VALID )
			{
				state = ST_COMPOSITE;
				cwin.composition += getAsciiFromVkey( vKey );
				cwin.candidates += lexicon->getCandidateString( getAsciiFromVkey( vKey ) );
			}
			else
			{
				state = ST_INIT;
				cwin.candidates = L"錯誤";
				cwin.composition = L"";
				lexicon->put( 0 );
			}

		}
		else if( vKey == VK_OEM_7 || vKey == VK_OEM_4 || vKey == VK_OEM_6 || vKey == VK_OEM_MINUS || vKey == VK_OEM_PLUS )
		{
			int index( 0 );

			if( vKey == VK_OEM_7 )  index = 0;
			else if( vKey == VK_OEM_4 )  index = 1;
            else if( vKey == VK_OEM_6 )  index = 2;
            else if( vKey == VK_OEM_MINUS )  index = 3;
            else if( vKey == VK_OEM_PLUS )  index = 5;

            vector<wchar_t> canvec = lexicon->getCandidates();

			if( index < canvec.size() )
			{
				state = ST_INIT;
//        	    cwin.composition = L"";
//				cwin.candidates = L"";

            	// commit to edit sessionn
				commit( pContext, canvec.at( index ) );
			}
//			else
//				*pfEaten = FALSE;

            cwin.candidates = L"";
       	    cwin.composition = L"";
            * pfEaten = TRUE;
			lexicon->put( 0 ); //reset
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

	}
	else
		* pfEaten = FALSE;

    cwin.redraw();
	return S_OK;
}
