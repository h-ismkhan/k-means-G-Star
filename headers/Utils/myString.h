#ifndef _MY_String_H
#define _MY_String_H

#include <string>
using namespace std;

#ifdef linux
char folder_sperator = '/';
#else
char folder_sperator = '\\';
#include <conio.h>
#endif

#ifdef linux
string command_dir_creator = "mkdir -p ";
#else
string command_dir_creator = "mkdir ";
#endif

template<class T>
T extract_name(T const & path, T const & delims = "/\\")
{
	return path.substr(path.find_last_of(delims) + 1);
}
template<class T>
T remove_extension(T const & filename)
{
	typename T::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}
template<class T>
T extract_name_minusExtension(T const & path, T const & delims = "/\\")
{
	return remove_extension(path.substr(path.find_last_of(delims) + 1));
}

#endif
