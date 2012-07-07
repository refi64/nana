/*
 *	A FileSystem Utility Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/filesystem/fs_utility.cpp
 *	@description:
 *		provide some interface for file managment
 */

#include <nana/filesystem/fs_utility.hpp>
#include <nana/filesystem/file_iterator.hpp>
#include <vector>
#if defined(NANA_WINDOWS)
	#include <windows.h>
#elif defined(NANA_LINUX)
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <dirent.h>
	#include <cstdio>
	#include <errno.h>
	#include <unistd.h>
#endif

namespace nana
{
namespace filesystem
{
//Because of No wide character version of POSIX
#if defined(NANA_LINUX)
	typedef std::string string_t;
	const char* splstr = "/\\";
#else
	typedef nana::string string_t;
	const nana::char_t* splstr = STR("/\\");
#endif
	//class path
		path::path(){}

		path::path(const nana::string& text)
#if defined(NANA_WINDOWS)
			:text_(text)
		{
#else
		{
			nana::stringset_cast(text_, text);
#endif
			string_t::size_type pos = text_.find_last_of(splstr);
			for(; pos != string_t::npos && (pos + 1 == text_.size()); pos = text_.find_last_of(splstr))
				text_.erase(pos);
		}

		bool path::empty() const
		{
#if defined(NANA_WINDOWS)
			return (::GetFileAttributes(text_.c_str()) == INVALID_FILE_ATTRIBUTES);
#elif defined(NANA_LINUX)
			struct stat sta;
			return (::stat(text_.c_str(), &sta) == -1);
#endif
		}

		path path::root() const
		{
		#if defined(NANA_WINDOWS)
			return path(filesystem::root(text_));
		#elif defined(NANA_LINUX)
			return path(filesystem::root(nana::stringset_cast(text_)));
		#endif
		}

		int path::what() const
		{
#if defined(NANA_WINDOWS)
			unsigned long attr = ::GetFileAttributes(text_.c_str());
			if(INVALID_FILE_ATTRIBUTES == attr)
				return type::not_exist;

			if(FILE_ATTRIBUTE_DIRECTORY & attr)
				return type::directory;

			return type::file;
#elif defined(NANA_LINUX)
			struct stat sta;
			if(-1 == ::stat(text_.c_str(), &sta))
				return type::not_exist;

			if((S_IFDIR & sta.st_mode) == S_IFDIR)
				return type::directory;

			if((S_IFREG & sta.st_mode) == S_IFREG)
				return type::file;

			return type::not_exist;
#endif
		}

		nana::string path::name() const
		{
			string_t::size_type pos = text_.find_last_of(splstr);
#if defined(NANA_WINDOWS)
			return text_.substr(pos + 1);
#else
			return nana::stringset_cast(text_.substr(pos + 1));
#endif
		}
	//private:
	//	nana::string text_;
	//};

	namespace detail
	{
		/*
		 *	rm_dir_recursive
		 *	@brief: remove a directory, if it is not empty, recursively remove it's subfiles and sub directories
		 */
		bool rm_dir_recursive(const nana::char_t* dir)
		{
			std::vector<file_iterator::value_type> files;
			nana::string path = dir;
			path += '\\';

			std::copy(file_iterator(dir), file_iterator(), std::back_inserter(files));

			std::vector<file_iterator::value_type>::iterator it = files.begin(), end = files.end();
			for(; it != end; ++it)
			{
				if(it->directory)
					rm_dir_recursive((path + it->name).c_str());
				else
					rmfile((path + it->name).c_str());
			}

			return rmdir(dir, true);
		}
	}

	long long filesize(const nana::char_t* file)
	{
#if defined(NANA_WINDOWS)
		//Some compilation environment may fail to link to GetFileSizeEx
		typedef BOOL (__stdcall *GetFileSizeEx_fptr_t)(HANDLE, PLARGE_INTEGER);
		GetFileSizeEx_fptr_t get_file_size_ex = reinterpret_cast<GetFileSizeEx_fptr_t>(::GetProcAddress(::GetModuleHandleA("Kernel32.DLL"), "GetFileSizeEx"));
		if(get_file_size_ex)
		{
			HANDLE handle = ::CreateFile(file, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if(INVALID_HANDLE_VALUE != handle)
			{
				LARGE_INTEGER li;
				if(!get_file_size_ex(handle, &li))
					li.QuadPart = 0;

				::CloseHandle(handle);
				return li.QuadPart;
			}
		}
		return 0;
#elif defined(NANA_LINUX)
		std::string mbstr;
		nana::stringset_cast(mbstr, file);
		FILE * stream = ::fopen(mbstr.c_str(), "rb");
		long long size = 0;
		if(stream)
		{
			fseeko64(stream, 0, SEEK_END);
			size = ftello64(stream);
			fclose(stream);
		}
		return size;
#endif
	}

	bool rmfile(const nana::char_t* file)
	{
#if defined(NANA_WINDOWS)
		bool ret = false;
		if(file)
		{
			ret = (::DeleteFile(file) == TRUE);
			if(!ret)
				ret = (ERROR_FILE_NOT_FOUND == ::GetLastError());
		}

		return ret;
#elif defined(NANA_LINUX)
		std::string mbstr;
		nana::stringset_cast(mbstr, file);
		if(std::remove(mbstr.c_str()))
			return (errno == ENOENT);
		return true;
#endif
	}

	bool rmdir(const nana::char_t* dir, bool fails_if_not_empty)
	{
		bool ret = false;
		if(dir)
		{
#if defined(NANA_WINDOWS)
			ret = (::RemoveDirectory(dir) == TRUE);
			if(!fails_if_not_empty && (::GetLastError() == ERROR_DIR_NOT_EMPTY))
				ret = detail::rm_dir_recursive(dir);
#elif defined(NANA_LINUX)
			std::string mbstr;
			nana::stringset_cast(mbstr, dir);
			if(::rmdir(mbstr.c_str()))
			{
				if(!fails_if_not_empty && (errno == EEXIST || errno == ENOTEMPTY))
					ret = detail::rm_dir_recursive(dir);
			}
			else
				ret = true;
#endif
		}
		return ret;
	}

	nana::string root(const nana::string& path)
	{
		size_t index = path.size();

		if(index)
		{
			const nana::char_t * str = path.c_str();

			for(--index; index > 0; --index)
			{
				nana::char_t c = str[index];
				if(c != '\\' && c != '/')
					break;
			}

			for(--index; index > 0; --index)
			{
				nana::char_t c = str[index];
				if(c == '\\' || c == '/')
					break;
			}
		}

		return index?path.substr(0, index + 1):nana::string();
	}

}//end namespace filesystem
}//end namespace nana
