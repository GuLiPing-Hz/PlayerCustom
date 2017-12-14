#ifndef CPLAYEREXCEPTION__H__
#define CPLAYEREXCEPTION__H__

#include <stdexcept>
#include <wchar.h>

#pragma warning( disable : 4290 )
class CPlayerException : public std::runtime_error
{
public:
	CPlayerException(const wchar_t* wmsg);
	CPlayerException(const CPlayerException& e);

	virtual ~CPlayerException(){}
	virtual const char * what() const;

	const wchar_t* getWWhat() const;
private:
	wchar_t wmsg[300];
};

#endif//CPLAYEREXCEPTION__H__
