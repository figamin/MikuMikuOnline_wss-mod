#include "GenerateJSON.hpp"
#include "../common/unicode.hpp"
#include <sys/stat.h>

namespace
{
	int ADFUNC_DXconvAnsiToWide(int destsize, WCHAR* wstrDest, const CHAR* strSrc )
	{
		// If strSrc is NULL, an error occurs
		if( strSrc==NULL )
			return 0;
		int ct = strlen(strSrc) + 1;
		if( wstrDest == NULL )
		// Changed the code page to handle Shift-JIS on non-Japanese locale
		// return MultiByteToWideChar( CP_ACP, 0, strSrc, ct, NULL, 0 );
			return MultiByteToWideChar( 932, 0, strSrc, ct, NULL, 0 );
		// int nResult = MultiByteToWideChar( CP_ACP, 0, strSrc, ct, wstrDest, destsize );
		int nResult = MultiByteToWideChar( 932, 0, strSrc, ct, wstrDest, destsize );
		wstrDest[destsize-1] = 0;

		if( nResult == 0 )
			return 0;
		return nResult;
	}

	TCHAR* _ftot_s( TCHAR* pString, size_t Size, float Value, int FracDigits )
	{
		if( pString == NULL )
			goto _error;
		TCHAR* pSource = pString;
		if( *(long*)&Value < 0 )
		{
			if( (size_t)( pString - pSource ) >= Size )
				goto _error;
			*pString++ = '-';
			*(long*)&Value ^= 0x80000000;
		}
		TCHAR* pBegin  = pString;
		int   Integer = (int)Value;
		for( int i = 10; i <= Integer; i *= 10, pString++ );
		if( (size_t)( pString - pSource ) >= Size )
			goto _error;
		TCHAR* pCode = pString++;
		*pCode-- = '0' + (char)( Integer % 10 );
		for( int i = Integer / 10; i != 0; i /= 10 )
			*pCode-- = '0' + (char)( i % 10 );
		if( FracDigits > 0 )
		{
			if( (size_t)( pString - pSource ) >= Size )
				goto _error;
			*pString++ = '.';
			while( FracDigits-- > 1 )
			{
				Value -= Integer;
				Integer = (int)( Value *= 10.0f );
				if( (size_t)( pString - pSource ) >= Size )
					goto _error;
				*pString++ = '0' + (char)Integer;
			}
			Integer = (int)( ( Value - Integer ) * 10.0f + 0.5f );
			if( Integer < 10 )
			{
				if( (size_t)( pString - pSource ) >= Size )
					goto _error;
				*pString++ = '0' + (char)Integer;
			}
			else
			{
				pCode = pString - 1;
				if( (size_t)( pString - pSource ) >= Size )
					goto _error;
				*pString++ = '0';
				do
				{
					if( *pCode == '.' )
						pCode--;
					if( *pCode != '9' )
					{
						*pCode += 1;
						break;
					}
					else
						*pCode = '0';
					if( pCode == pBegin )
					{
						if( (size_t)( pString - pSource ) >= Size )
							goto _error;
						TCHAR* pEnd = pString++;
						while( pEnd-- != pCode )
							*( pEnd + 1 ) = *pEnd;
						*pCode = '1';
					}
				} while( *pCode-- == '0' );
			}
		}
		if( (size_t)( pString - pSource ) >= Size )
			goto _error;
		*pString = '\0';
		return pSource;
_error:
		__debugbreak();
		return NULL;
	}

	BOOL DeleteDirectory( LPCTSTR lpPathName )
	{
		// Input value check
		if( NULL == lpPathName )
		{
			return FALSE;
		}

		// Save directory path (add / if the directory doesn't exist)
		TCHAR szDirectoryPathName[_MAX_PATH];
		_tcsncpy_s( szDirectoryPathName, _MAX_PATH, lpPathName, _TRUNCATE );
		if( '/' != szDirectoryPathName[_tcslen(szDirectoryPathName) - 1] )
		{	// If there is no / at the end, add one
			_tcsncat_s( szDirectoryPathName, _MAX_PATH, _T("/"), _TRUNCATE );
		}

		// File name search pattern creation
		TCHAR szFindFilePathName[_MAX_PATH];
		_tcsncpy_s( szFindFilePathName, _MAX_PATH, szDirectoryPathName, _TRUNCATE );
		_tcsncat_s( szFindFilePathName, _MAX_PATH, _T("*"), _TRUNCATE );

		// Start scanning
		WIN32_FIND_DATA		fd;
		HANDLE hFind = FindFirstFile( szFindFilePathName, &fd );
		if( INVALID_HANDLE_VALUE == hFind )
		{	// The directory does not exist
			return FALSE;
		}

		do
		{
			// if( '.' != fd.cFileName[0] )
			// Current folder (.)
			if( 0 != _tcscmp( fd.cFileName, _T(".") )
				// Parent folder (..)
				&& 0 != _tcscmp( fd.cFileName, _T("..") ) )	
			{
				TCHAR szFoundFilePathName[_MAX_PATH];
				_tcsncpy_s( szFoundFilePathName, _MAX_PATH, szDirectoryPathName, _TRUNCATE );
				_tcsncat_s( szFoundFilePathName, _MAX_PATH, fd.cFileName, _TRUNCATE );

				if( FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes )
				{
					// If it's a directory, recursively delete
					if( !DeleteDirectory( szFoundFilePathName ) )
					{
						FindClose( hFind );
						return FALSE;
					}
				}
				else
				{
					// Else, it's a file and delete with the Win32 API
					if( !DeleteFile( szFoundFilePathName ) )
					{
						FindClose( hFind );
						return FALSE;
					}
				}
			}
		} while( FindNextFile( hFind, &fd ) );

		FindClose( hFind );

		return RemoveDirectory( lpPathName );
	}

	// int Trim(char *s) {
	int Trim(char *s,unsigned int size) {
    int i;
    int count = 0;

    // NULL pointer?
    if ( s == NULL ) {
		// yes
        return -1;
    }

    // Get string length
    i = strlen(s);

    // Search for non space characters from the end
    while ( --i >= 0 && s[i] == ' ' ) count++;

    // Adding a null terminator
    s[i+1] = '\0';

    // Search for non space characters from the beginning
    i = 0;
    while ( s[i] != '\0' && s[i] == ' ' ) i++;
	// strcpy(s, &s[i]);
    strcpy_s(s,size, &s[i]);

    return i + count;
	}
};

#define MAX_PATH_L (MAX_PATH * 2)

JsonGen::JsonGen()
{
	HANDLE hFind,hPmdFind,hTxtFind;
	// Directory Pmd Text
	WIN32_FIND_DATA win32fd_dir,win32fd_pmd,win32fd_txt;
	// Directory Pmd Text
	TCHAR tcsTmpPath[MAX_PATH_L] = {0};
	TCHAR tcsTmpPath_Pmd[MAX_PATH_L] = {0};
	TCHAR tcsTmpDir[MAX_PATH_L] = {0};

	std::vector<std::wstring> pmd_paths;
	std::wstring prejson;
	DxLib::VECTOR prePos = {0},curPos = {0},lclPos = {0};
	DxLib::MATRIX chglcl = {0};

	char pmd_model_name_[MAX_PATH_L] = {0};
	TCHAR pmd_author_name_[MAX_PATH_L] = {0};
	// Added for PMX compatibility
	std::wstring pmx_model_name_;  
	std::wstring pmx_author_name_;


	int exist_num_pmd_ = 0;

	int model_handle_ = 0;

	FILE *json_file = 0;

	// Search in systems/models
	hFind = FindFirstFile(_T("./models/?*"), &win32fd_dir);

	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		if (win32fd_dir.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
			_tcscmp(win32fd_dir.cFileName,_T("."))!=0 &&
			_tcscmp(win32fd_dir.cFileName,_T(".."))!=0 &&
			_tcscmp(win32fd_dir.cFileName,_T(".svn"))!=0) {
			_tcscpy_s(tcsTmpDir,_T("./models/"));
			_tcscat_s(tcsTmpDir,win32fd_dir.cFileName);
			_tcscat_s(tcsTmpDir,_T("/"));

			ZeroMemory(tcsTmpPath,MAX_PATH_L);
			_tcscpy_s(tcsTmpPath,tcsTmpDir);
			_tcscat_s(tcsTmpPath,_T("info.json"));

			// If info.json doesn't exist, create it according to PMD
			if(!PathFileExists(tcsTmpPath))
			{
				ZeroMemory(tcsTmpPath_Pmd,MAX_PATH_L);
				_tcscpy_s(tcsTmpPath_Pmd,tcsTmpDir);
				// _tcscat_s(tcsTmpPath_Pmd,_T("*.pmd"));
				_tcscat_s(tcsTmpPath_Pmd,_T("*.pm?"));
				// Changed to support pmx
				hPmdFind = FindFirstFile(tcsTmpPath_Pmd, &win32fd_pmd);
				// if(hPmdFind == (HANDLE)0xffffffff)
				if(hPmdFind == INVALID_HANDLE_VALUE)
				// Fixed to prevent 64 bit build problems 
				{
					FindClose(hPmdFind);
					continue;
				}

				TCHAR cur_dir[MAX_PATH_L];
				ZeroMemory(cur_dir,MAX_PATH_L);
				GetCurrentDirectory(MAX_PATH_L,cur_dir);
				bool flag = false;
				/*
				for(int k = MAX_PATH_L - 1;k != 0;--k)
				{
					if( cur_dir[k] == _T('/') )
					{
						cur_dir[k] = 0;
						break;
					}
				}
				*/
				do {
					std::wstring tmp_path = cur_dir;
					tmp_path += _T("/models/");
					tmp_path += win32fd_dir.cFileName;
					tmp_path += _T("/");
					tmp_path += win32fd_pmd.cFileName;
					pmd_paths.push_back(tmp_path);
				} while (FindNextFile(hPmdFind, &win32fd_pmd));
				FindClose(hPmdFind);

				for(unsigned int i = 0;i < pmd_paths.size();++i){
					// Changed to support pmx
					if (pmd_paths[i].substr(pmd_paths[i].size()-4,4) == L".pmx") {
						// PMX
						// int fd = _topen(pmd_paths[i].c_str(),O_RDONLY);
						int fd;
						_tsopen_s(&fd,pmd_paths[i].c_str(),_O_BINARY|_O_RDONLY,_SH_DENYWR,_S_IREAD|_S_IWRITE);
						char readbuf[1024];
						_read(fd,readbuf,9);
						// Header check
						if( readbuf[ 0 ] != 'P' || readbuf[ 1 ] != 'M' || readbuf[ 2 ] != 'X' || readbuf[ 3 ] != ' ' ) {
							_close(fd);
							continue;
						}
						// Check if it's Ver2.0
						if( readbuf[ 4 ] != 0x00 || readbuf[ 5 ] != 0x00 || readbuf[ 6 ] != 0x00 || readbuf[ 7 ] != 0x40 ) {
							_close(fd);
							continue;
						}

						// Get the size of the header and read it
						auto AddHeadDataSize = readbuf[ 8 ] ;
						_read(fd,readbuf,8);
						if (AddHeadDataSize > 8) {
							// If the header is larger than 8 bytes, skip the rest
							_lseek(fd,AddHeadDataSize - 8,SEEK_CUR);
						}
						auto EncodeType =  readbuf[0];
						// Encoding methods: 
						// 0:UTF16
						// 1:UTF8

						// Model information loading
						DWORD size;
						_read(fd,(char *) &size,4);
						// Get text size
						if (size <= 1024) {
							// Read string info
							_read(fd,readbuf,size);
						} else {
							// If greater than 1Kb, read it and skip the rest
							// Read string info
							_read(fd,readbuf,1024); 
							_lseek(fd,size - 1024,SEEK_CUR);
							size=1024;
						}
						// Get model name
						pmx_model_name_ = L"";
						switch (EncodeType) {
						// UTF16
						case 0:
							for (int idx=0;idx<(int)size;idx+=2) {
								pmx_model_name_ +=  (unsigned char)readbuf[idx] + ((unsigned char)readbuf[idx+1]<<8);
							}
							break;
						// UTF8
						case 1:
							if (size < 1024) {
								readbuf[size] = 0x00;
							} else {
								readbuf[1023] = 0x00;
							}
							pmx_model_name_ = unicode::ToWString(readbuf);
							break;
						}
						// Skip model English name
						// Get text data size
						_read(fd,(char *) &size,4);  
						_lseek(fd,size,SEEK_CUR);

						// Get comments
						// Get text data size
						_read(fd,(char *) &size,4);  
						if (size <= 1024) {
							// Read string info
							_read(fd,readbuf,size);
						} else {
							// If greater than 1Kb, read it and skip the rest
							// Read string info
							_read(fd,readbuf,1024);
							_lseek(fd,size - 1024,SEEK_CUR);
							size=1024;
						}
						_close(fd);

						std::wstring pmx_model_comment = L"";
						switch (EncodeType) {
						// UTF16
						case 0:
							for (int idx=0;idx<(int)size;idx+=2) {
								pmx_model_comment +=  (unsigned char)readbuf[idx] + ((unsigned char)readbuf[idx+1]<<8);
							}
							break;
						// UTF8
						case 1: 
							if (size < 1024) {
								readbuf[size] = 0x00;
							} else {
								readbuf[1023] = 0x00;
							}
							pmx_model_comment = unicode::ToWString(readbuf);
							break;
						}

						// Get model author
						pmx_author_name_ = L"Unknown";
						unsigned int cnt = 0;
						while(cnt < pmx_model_comment.size()){
							// This means "Modeling"
							if(pmx_model_comment.substr(cnt,5) == L"モデリング") break;
							else	++cnt;
						}
						if(cnt != pmx_model_comment.size()) {
							for(cnt;cnt<pmx_model_comment.size() && pmx_model_comment.substr(cnt,1) != L"：" && pmx_model_comment.substr(cnt,1) != L":"; ++cnt);
							for(cnt;cnt<pmx_model_comment.size() && (pmx_model_comment.substr(cnt+1,1) == L" "||pmx_model_comment.substr(cnt+1,1) == L"　"); ++cnt);
							int tmp_str_cnt = 1;
							for(tmp_str_cnt;cnt+tmp_str_cnt<pmx_model_comment.size() && (pmx_model_comment.substr(cnt+tmp_str_cnt,1) !=L"\n" && pmx_model_comment.substr(cnt+tmp_str_cnt,1) !=L"\r" && pmx_model_comment.substr(cnt+tmp_str_cnt,1) != L" " && pmx_model_comment.substr(cnt+tmp_str_cnt,1) != L".");++tmp_str_cnt);
							// Get author
							pmx_author_name_ = pmx_model_comment.substr(cnt+1,tmp_str_cnt-1);
						}

					} else if (pmd_paths[i].substr(pmd_paths[i].size()-4,4) == L".pmd") {
						// PMD
						char pmd_info[PMDINFO_SIZE+1];
						// int fd = _topen(pmd_paths[i].c_str(),O_RDONLY);
						int fd;
						_tsopen_s(&fd,pmd_paths[i].c_str(),_O_BINARY|_O_RDONLY,_SH_DENYWR,_S_IREAD|_S_IWRITE);
						_read(fd,pmd_info,PMDINFO_SIZE);
						_close(fd);

						// Get model name
						strcpy_s(pmd_model_name_,pmd_info+7);
						// Trim(pmd_model_name_);
						Trim(pmd_model_name_,sizeof(pmd_model_name_));
						unsigned int cnt = 0x1b;
						size_t info_size = ADFUNC_DXconvAnsiToWide(0,0,pmd_info+cnt);
						TCHAR *pmd_info_t = new TCHAR[info_size + 1];
						ADFUNC_DXconvAnsiToWide(info_size,pmd_info_t,pmd_info+cnt);
						cnt = 0;
						while(cnt < info_size){
							// This means "Modeling"
							if(!_tcsncmp(_T("モデリング"),pmd_info_t+cnt,5))break;
							else	++cnt;
						}
						if(cnt == info_size)
						{
							_tcscpy_s(pmd_author_name_,_T("Unknown"));
						} else {
							// for(cnt;cnt<info_size && *(pmd_info_t+cnt) != _T('：'); ++cnt);
							for(cnt;cnt<info_size && *(pmd_info_t+cnt) != _T('：') && *(pmd_info_t+cnt) != _T(':'); ++cnt);
							// Added check for half width ":"
							for(cnt;cnt<info_size && (*(pmd_info_t+cnt+1) == _T(' ')||*(pmd_info_t+cnt+1) == _T('　')); ++cnt); 
							// Add spaces
							int tmp_str_cnt = 1;
							for(tmp_str_cnt;cnt+tmp_str_cnt<info_size && (*(pmd_info_t+cnt+tmp_str_cnt) != _T('\n') && *(pmd_info_t+cnt+tmp_str_cnt) != _T(' ') && *(pmd_info_t+cnt+tmp_str_cnt) != _T('.'));++tmp_str_cnt);
							// Get author
							_tcsncpy_s(pmd_author_name_,pmd_info_t+cnt+1,tmp_str_cnt-1);
						}
						delete []pmd_info_t;
					// Changed to support PMX
					} else {
						// If other than PMX or PMD, exit the loop
						continue;
					}
						TCHAR tmp_mv1_path[MAX_PATH_L] = {0};
						_tcscpy_s(tmp_mv1_path,pmd_paths[i].c_str());

						model_handle_ = MV1LoadModel( tmp_mv1_path );
						// Changed to not process if model load failed
						if (model_handle_ == -1) {
							continue;
						}
						int tex_num = MV1GetTextureNum( model_handle_ );
						int frame_num = MV1GetFrameNum( model_handle_ );


						// Setting model height based on bones
						for(int j = 0;j < frame_num;++j)
						{
							curPos = MV1GetFramePosition( model_handle_, j );
							chglcl = MV1GetFrameLocalMatrix( model_handle_, j );
							lclPos.x = chglcl.m[3][0];
							lclPos.y = chglcl.m[3][1];
							lclPos.z = chglcl.m[3][2];

							if(	prePos.y < curPos.y &&
								lclPos.x == curPos.x &&
								lclPos.y == curPos.y &&
								lclPos.z == curPos.z)prePos = curPos;
						}
					// Building JSON
					prejson = _T("{\n\t\"name\": \"");
					std::wstring premodelname = _T("char:");

					// Changed to support PMX
					if (pmd_paths[i].substr(pmd_paths[i].size()-4,4) == L".pmx") {
						premodelname += pmx_model_name_;
						premodelname += _T(":");
						premodelname += pmx_author_name_;
					} else if (pmd_paths[i].substr(pmd_paths[i].size()-4,4) == L".pmd") { 
						size_t tmp_w_s_m = ADFUNC_DXconvAnsiToWide(0,0,pmd_model_name_);
						TCHAR *tmp_w_m = new TCHAR[tmp_w_s_m + 1];
						ADFUNC_DXconvAnsiToWide(tmp_w_s_m,tmp_w_m,pmd_model_name_);
						premodelname += tmp_w_m;
						premodelname += _T(":");
						premodelname += pmd_author_name_;
						// Changed to support PMX
						// This was used later and was out of scope, so it was put here lol
						pmx_model_name_ = tmp_w_m;
						// The temp object is released
						delete [] tmp_w_m;
					}
					premodelname += _T("式");

					// If greater than 64b, read it and skip the rest
					auto premodelname_ = unicode::ToString(premodelname);
                    if (premodelname_.size() > 64) {
                        for (int i=20;i<=(int)premodelname.size();i++){
                            premodelname_ = unicode::ToString(premodelname.substr(0,i));
                            if (premodelname_.size() > 64){
                                premodelname = premodelname.substr(0,i-1);
                                break;
                            }
                        }
                    }
					prejson += premodelname;
					prejson += _T("\",\n\t\"character\":\n\t\t{\n\t\t\t\"height\":");
					TCHAR tmp_f[32];
					_ftot_s(tmp_f,32,floor(prePos.y*2)/10.0f,2);
					// If the height is 0, change to 1.6
					if (_tcscmp(tmp_f,_T("0.00"))==0){
						lstrcpy(tmp_f, _T("1.60"));
					}
					prejson += tmp_f;
					prejson += _T(",\n\t\t\t\"motions\":\n\t\t\t\t{\n\t\t\t\t\t\"stand\":\"basic_stand.vmd\",\n\t\t\t\t\t\"walk\": \t\"basic_walk.vmd\",\n\t\t\t\t\t\"run\":\t\"basic_run.vmd\"\n\t\t\t\t}\n\t\t}\n}");
					TCHAR tmp_dir[MAX_PATH_L];
					_tcscpy_s(tmp_dir,_T("./models/"));


					// Changed to support PMX
					if (pmd_paths[i].substr(pmd_paths[i].size()-4,4) == L".pmx") {
						_tcscat_s(tmp_dir,pmx_author_name_.c_str());
						_tcscat_s(tmp_dir,_T("式"));
						_tcscat_s(tmp_dir,_T("/"));
						if(!PathIsDirectory(tmp_dir)){
							_wmkdir(tmp_dir);
						}
						_tcscpy_s(tmp_dir,_T("./models/"));
						_tcscat_s(tmp_dir,pmx_author_name_.c_str());
						_tcscat_s(tmp_dir,_T("式"));
						_tcscat_s(tmp_dir,_T("/"));
					} else if (pmd_paths[i].substr(pmd_paths[i].size()-4,4) == L".pmd") { 
						_tcscat_s(tmp_dir,pmd_author_name_);
						_tcscat_s(tmp_dir,_T("式"));
						_tcscat_s(tmp_dir,_T("/"));
						if(!PathIsDirectory(tmp_dir)){
							_wmkdir(tmp_dir);
						}
						_tcscpy_s(tmp_dir,_T("./models/"));
						_tcscat_s(tmp_dir,pmd_author_name_);
						_tcscat_s(tmp_dir,_T("式"));
						_tcscat_s(tmp_dir,_T("/"));
						// Changed to support PMX
						// _tcscat_s(tmp_dir,tmp_w_m);
						// Since it's out of scope, assign to pmx_model_name_ and use it
					}
					// If the folder name ends with ".", remove it
					while (pmx_model_name_.rfind('.')==pmx_model_name_.length()-1){ 
						
						pmx_model_name_.erase(pmx_model_name_.length()-1,1);
					}
					_tcscat_s(tmp_dir,pmx_model_name_.c_str());
					_tcscat_s(tmp_dir,_T("/"));
					_wmkdir(tmp_dir);
					// delete [] tmp_w_m;
					// Since it's out of scope, release it within the scope and comment out
					// delete [] tmp_w_a;
					TCHAR json_path[MAX_PATH_L];
					_tcscpy_s(json_path,tmp_dir);
					_tcscat_s(json_path,_T("info.json"));
					_tfopen_s(&json_file,json_path, _T("w, ccs=UTF-8"));
					// File opening may fail with an error, so fix it
					// fseek(json_file, 0, SEEK_SET);
					// _ftprintf_s(json_file,prejson.c_str());
					// fclose(json_file);
					if (json_file != 0) {
						fseek(json_file, 0, SEEK_SET);
						_ftprintf_s(json_file,prejson.c_str());
						fclose(json_file);
					}

					TCHAR tmp_src[MAX_PATH_L];
					TCHAR tmp_cpy[MAX_PATH_L];

					// Copy textures
					for(int t = 0;t < tex_num;++t)
					{
						_tcscpy_s(tmp_src,tcsTmpDir);
						_tcscat_s(tmp_src,MV1GetTextureName( model_handle_, t));
						_tcscpy_s(tmp_cpy,tmp_dir);
						_tcscat_s(tmp_cpy,MV1GetTextureName( model_handle_, t));
						// Create directories if they don't exist
						for (unsigned int idx = 0;idx<sizeof(tmp_cpy) && *(tmp_cpy+idx) != _T('\0');idx++) {
							if (*(tmp_cpy+idx) == _T('\\')||*(tmp_cpy+idx) == _T('/')) {
								TCHAR dirchk[MAX_PATH_L];
								_tcsncpy_s(dirchk,tmp_cpy,idx+1);
								if(!PathIsDirectory(dirchk)){
									_wmkdir(dirchk);
								}
							}
						}
						CopyFile(tmp_src,tmp_cpy,TRUE);
					}
					TCHAR tmp_txt_f[MAX_PATH_L];
					_tcscpy_s(tmp_txt_f,tcsTmpDir);
					_tcscat_s(tmp_txt_f,_T("*.txt"));
					hTxtFind = FindFirstFile(tmp_txt_f,&win32fd_txt);
					do{
						_tcscpy_s(tmp_src,tcsTmpDir);
						_tcscat_s(tmp_src,win32fd_txt.cFileName);
						_tcscpy_s(tmp_cpy,tmp_dir);
						_tcscat_s(tmp_cpy,win32fd_txt.cFileName);
						CopyFile(tmp_src,tmp_cpy,TRUE);
					}while(FindNextFile(hTxtFind, &win32fd_txt));
					FindClose(hTxtFind);
					// If there is a .vmd motion file, copy it
					_tcscpy_s(tmp_txt_f,tcsTmpDir);
					_tcscat_s(tmp_txt_f,_T("*.vmd"));
					hTxtFind = FindFirstFile(tmp_txt_f,&win32fd_txt);
					do{
						_tcscpy_s(tmp_src,tcsTmpDir);
						_tcscat_s(tmp_src,win32fd_txt.cFileName);
						_tcscpy_s(tmp_cpy,tmp_dir);
						_tcscat_s(tmp_cpy,win32fd_txt.cFileName);
						CopyFile(tmp_src,tmp_cpy,TRUE);
					}while(FindNextFile(hTxtFind, &win32fd_txt));
					FindClose(hTxtFind);
					TCHAR tmp_type[128] = {0};
					// _tsplitpath(pmd_paths[i].c_str(),NULL,NULL,tmp_cpy,tmp_type);
					_tsplitpath_s(pmd_paths[i].c_str(),NULL,0,NULL,0,tmp_cpy,sizeof(tmp_cpy)/sizeof(tmp_cpy[0]),tmp_type,sizeof(tmp_type)/sizeof(tmp_type[0]));
					_tcscat_s(tmp_cpy,tmp_type);
					_tcscpy_s(tmp_type,tmp_dir);
					_tcscat_s(tmp_type,tmp_cpy);
					CopyFile(pmd_paths[i].c_str(),tmp_type,TRUE);
				}
				pmd_paths.clear();
				// Do not delete if directly under a character
				// This means "character"
				if (_tcscmp(tcsTmpDir,_T("./models/キャラクター/")) !=0){
					DeleteDirectory(tcsTmpDir);
				}
			}
		}
	} while (FindNextFile(hFind, &win32fd_dir));

	FindClose(hFind);
}