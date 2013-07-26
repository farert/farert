
#include "stdafx.h"


#if defined _DEBUG

FILE *os;
Route route;


/////////////////////////////////////////////////////////////////////////////////////
static void setup_route(TCHAR* route_str)
{
	TCHAR* p;
	int lineId = 0;
	int stationId1 = 0;
	int stationId2 = 0;
	TCHAR* ctx = NULL;

	for (p = _tcstok_s(route_str, _T(", "), &ctx); p; p = _tcstok_s(NULL, _T(", "), &ctx)) {
		if (stationId1 == 0) {
			route.startStationId = stationId1 = Route::GetStationId(p);
			ASSERT(0 < stationId1);
		} else if (lineId == 0) {
			lineId = Route::GetLineId(p);
			ASSERT(0 < lineId);
		} else {
			stationId2 = Route::GetStationId(p);
			ASSERT(stationId2);
			VERIFY(0 <= route.add(lineId, stationId1, stationId2));
			lineId = 0;
			stationId1 = stationId2;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
static void show_route()
{
	CString lineName;
	int lineId;
	CString stationName;
	int stationId;

	stationName = Route::StationName(route.startStationId);
	_ftprintf(os, _T("start:  %s\n"), stationName);

	vector<RouteItem>::const_iterator pos = route.routeList().cbegin();

	ASSERT((pos->lineId == 0) && (pos->stationId == route.startStationId));
	pos++;
	for (; pos != route.routeList().cend() ; pos++) {

		stationId = pos->stationId;
		if (stationId != route.endStationId) {
			stationName = Route::StationName(stationId);
		}
		lineId = pos->lineId;
		lineName = route.LineName(lineId);

		_ftprintf(os, _T("%s, %s\n"), lineName, stationName);
	}
}

/////////////////////////////////////////////////////////////////////////////////////

static void test_a69(void)
{
	TCHAR *route_def[] = {
	 _T("���l,���C����,����,���t��,�h��,�O�[��,���,������,����,������,���q,���c��,�䑷�q,��֐�,���"),
	 _T("���l,���C����,����,���t��,�h��"),
	 _T("���l,���C����,����,���t��,�h��,�O�[��, ��t, ���c��, ���c"),
	 _T("���l,���C����,����,���t��,�h��,�O�[��, ��t, ������, �D��"),
	 _T("���,���C����,���É�"),
	 _T("����, ���ِ�, ���, ���ِ�(������), �X, ���ِ�, ����, �x�ǖ��, �x�ǖ�"),
	 _T("�ԑ�, �Ζk��, �V����, �@�J��, ����, ���ِ�, �X, ���ِ�(������), ���, ���ِ�, �ܗŊs"),
	 _T("�F�s�{,���k��,��{,���k��(�鋞��),�ԉH,���k��(���v�o�R),���闢,��֐�,����,���S��,�}��"),
	 _T("�F�s�{,���k��,��{,���k��(�鋞��),�ԉH,���k��(���v�o�R),���闢,���k��,����,���t��,�h��,�O�[��,�Ό�"),
	 _T("�F�s�{,���k��,��{,���k��(�鋞��),�����Y�a,�������,�V����,��֐�,�o�t"),
	 _T("�F�s�{,���k��,��{,���k��(�鋞��),�����Y�a,�������,�{���{��,�앐��,���"),
	 _T("�F�s�{,���k��,��{,���k��(�鋞��),�ԉH,���k��,��Y�a,�������,�����Y�a"),
	 _T("���,���k��,�ԉH,���k��(�鋞��),��{,��z��,�����,������,�q��"),
	 _T("���,���k��,�ԉH,���k��(�鋞��),��{,���k��,�k��,�k���,�ق��Ƃ䂾"),
	 _T("����,���C����,�i��,���C����(�����o�R),�ߌ�,���C����,���l"),
	 _T("����,���t��,�h��,�O�[��,���Y"),
	 _T("���,���C����,�Č�,�k����,����"),
	 _T("����,�R�z��,�O��,����,�C�c�s,�R�z��,�L��"),
	 _T("�L��,�R�z��,�V�R��"),
	 _T("�_��,�R�z��,�O��,����,�C�c�s,�R�z��,����"),
	 _T("�O��,����,�C�c�s,�R�z��,����"),
	 _T("�O��,����,�C�c�s"),
	 _T("�_��,�R�z��,�O��,����,�C�c�s"),
	 _T("����,�R�z��,�_��"),
	 _T("�����l,�R�z��,�_��"),
	 _T("����,�R�z��,�����l"),
	 _T("�����l,�R�z��,�⍑"),
	 _T("����,�R�z��,�⍑"),
	 _T("�_��,�R�z��,����"),
	 _T("�h��,���t��,����"),
	 _T("���Y,�O�[��,�h��,���t��,����"),
	 _T("����,���t��,�h��,�O�[��,���Y"),
	 _T("����,���t��,�h��,���[��,�����Y"),
	 _T("����,���t��,�h��"),
	 _T("����,������,��t, �O�[��,���Y"),
	 _T("���Y,�O�[��,��t, ������,����"),
	 _T("�ю���,������(�ю���-�䒃�m��),�䒃�m��,��������, ����, ���t��, ��t, �O�[��,���Y"),
	 _T("����,������(�ю���-�䒃�m��),�䒃�m��,��������, ����, ���t��, ��t, �O�[��,���Y"),
	 _T("���,���C����,����,���t��,�h��"),
	 _T("���,���C����,�Č�,�k����,���]��"),
	 _T("�R��,���C����,�Č�,�k����,���]��"),
	 _T("�R��,���C����,�Č�,�k����,�ߍ]����"),		// NG
	 _T("���,���C����,�Č�,�k����,�ߍ]����"),
	 _T("���,���C����,���É�"),
	 _T("���É�,���C����,���"),
	 _T("���É�,���C����,�Č�,�k����,����"),
	 _T("���É�,���C����,�Č�"),
	 _T("�։�,�k����,�Č�,���C����,���É�"),
	 _T("���]��,�k����,�Č�,���C����,���"),
	 _T("���]��,�k����,�ߍ]����,�ΐ���, �R��, ���C����,���"),
	 _T("���,���C����, �R��, �ΐ���, �ߍ]����, �k����, ���]��"),
	 _T("�ߍ]����,�ΐ���, �R��"),
	 _T("�R��, �ΐ���, �ߍ]����"),
	 _T("���]��,�k����,�ߍ]����,�ΐ���, �R��"),
	 _T("�ߍ]����,�ΐ���, �R��, ���C����,���"),
	 _T("�R��, �ΐ���, �ߍ]����, �k����, ���]��"),
	 _T("���,���C����, �R��, �ΐ���, �ߍ]����"),
	 _T("�ߍ]����,�k����,�Č�,���C����,���"),
	 _T("�ߍ]����,�k����,�Č�"),
	 _T("����,�k����,�Č�"),
	 _T("�Č�,�k����,����"),
	 _T("�ߍ]����,�k����,�Č�,���C����,�R��"),		// NG
	 _T("���]��,�k����,�Č�,���C����,�R��"),
	 _T("����,�R�z��,�⍑,�⓿��,����"),
	 _T("����,�R�z��,�����l,�⓿��,����"),
//	 _T(",,,,,,,,,"),
	_T(""),
	};
	TCHAR buffer[1024];
	int i;
	vector<RouteItem> replace_route;
	for (i = 0; '\0' != *route_def[i]; i++) {
		route.removeAll();
		replace_route.clear();
		_tcscpy_s<1024>(buffer, route_def[i]);
		setup_route(buffer);
		int cnt = Route::ReRouteRule69j(route.routeList(), &replace_route);
		show_route();
		_ftprintf(os, _T(">>>>>>>>>>>%d\n"), cnt);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		show_route();
		_ftprintf(os, _T("-----------\n\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////////////
static void test_a86(void)
{
	TCHAR *route_def[] = {
    _T("���o,������������,�v��,�֐���,���q,�a�̎R��,�a�̎R,�I����,�T�R,�֐���,���É�,���C����,���R(��)"),
	_T("���R(��),���C����,���É�,�֐���,�T�R,�I����,�a�̎R,�a�̎R��,���q,�֐���,�v��,������������,���o"),
	_T("����,��������,���K,��������,���R(��),���C����,���É�"),
	_T("����,��������,���K,��������,���R(��),���C����,���É�"),
	_T("�i��,�R���,��X��,��������,���K,��������,���R(��),���C����,���É�"),
	_T("���c,���C����,�i��,�R���,��X��,��������,���K,��������,���R(��),���C����,���É�"),
	_T("����,��������,���K,��������,���R(��)"),
	_T("����,��������,���K,��������,���R(��)"),
	_T("�i��,�R���,��X��,��������,���K,��������,���R(��)"),
	_T("���c,���C����,�i��,�R���,��X��,��������,���K,��������,���R(��)"),
	_T("���c,���C����,�i��,�R���,��X��,��������,�䒃�m��,������(�ю���-�䒃�m��),�H�t��,���k��,�c�[,�R���,�r��,�ԉH��(�鋞��),�ԉH,���k��(�鋞��),��{,�����,����,��z��,�{��,�M�z��(�m��-�V��),�V��,�H�z��,�H�c"),
	_T("�H�c,�H�z��,�V��,�M�z��(�m��-�V��),�{��,��z��,����,�����,��{,���k��(�鋞��),�ԉH,�ԉH��(�鋞��),�r��,�R���,�c�[,���k��,�H�t��,������(�ю���-�䒃�m��),�䒃�m��,��������,��X��,�R���,�i��,���C����,���c"),
	_T("���c,���C����,�i��,�R���,��X��,��������,�䒃�m��,������(�ю���-�䒃�m��),�H�t��,���k��,����"),
	_T("����,���k��,�H�t��,������(�ю���-�䒃�m��),�䒃�m��,��������,��X��,�R���,�i��,���C����,���c"),
	_T("���c,���C����,����,���k��,����"),
	_T("�ԉH,���k��,����"),
	_T("�ԉH,���k��(�鋞��),��{,���k��,����"),
	_T("���c,���C����,����,��������,��������,�������,�����Y�a,���k��(�鋞��),��{,���k��,����"),
	_T("���ԏM�n,���k��(�鋞��),�ԉH,�ԉH��(�鋞��),�r��,�R���,�V�h,��������,���{"),
	_T("���ԏM�n,���k��(�鋞��),��{,���k��,����"),
	_T("����,���k��,��{,���k��(�鋞��),���ԏM�n"),
	_T("���ԏM�n,���k��(�鋞��),�ԉH,���k��,����"),
	_T("���ԏM�n,���k��(�鋞��),�����Y�a,�������,��Y�a,���k��,����"),
	_T("���ԏM�n,���k��(�鋞��),�����Y�a,�������,�V����,��֐�,���,���k��,����"),
	_T("��,�ԉH��(�鋞��),�ԉH,���k��(���v�o�R),���闢,���k��,����,���C���V����,���É�"),
	_T("��,�ԉH��(�鋞��),�ԉH,���k��(���v�o�R),���闢,���k��,�H�t��,������(�ю���-�䒃�m��),�䒃�m��,��������,��X��,�R���,�i��,���C���V����,���É�"),
	_T("���c,���C����,���,�앐��,����,��������,���{"),
	_T("�����,���C����(�����o�R),��������,�앐��,�{���{��,�������,�V����,��֐�,���"),
	 _T("�����̗�����,�D����,�K��,���ِ�,�ܗŊs,�]����,�،Ó�,�C����,������,�Ìy��,�X,���H��,�V�X,���k�V����, ����,���t��,�s�쉖�l,���t��(�s�쉖�l-���D��),���D��,������,�ю���"),
	 _T("�a�J,�R���,���c�n��"),
	 _T("����,���C��,�C�K"),
	 _T("�V�\�Ð�,�D����,�K��,���ِ�,�ܗŊs,�]����,�،Ó�,�C����,������,�Ìy��,�X,���H��,�V�X,���k�V����, ����,���t��,�s�쉖�l,���t��(�s�쉖�l-���D��),���D��,�������,��Y�a"),
	 _T("�����E,��������,�V�h,�R���,�r��,�ԉH��(�鋞��),�ԉH,���k��,����,���t��,�����ՊC����"),
	 _T("���l,���C����,����,���t��,�h��,�O�[��,���,������,����,������,���q,���c��,�䑷�q,��֐�,���"),
	 _T("���Óc,���l��,�V���l,���C���V����,���É�"),
	 _T("���Óc,���l��,�V���l,���C���V����,�O��"),
	 _T("�V���l,���C���V����,�O��"),
	 _T("�i��,�R���,��X��,��������,�䒃�m��,������(�ю���-�䒃�m��),�H�t��,���k��,�c�[,�R���,�r��,�ԉH��(�鋞��),�ԉH,���k��(�鋞��),��{,���k��,����"),
	 _T("�����̗�����,�D����,�K��,���ِ�,�ܗŊs,�]����,�،Ó�,�C����,������,�Ìy��,�X,���H��,�V�X,���k�V����, ����"),
	 _T("�����̗�����,�D����,�K��,���ِ�,�ܗŊs,�]����,�،Ó�,�C����,������,�Ìy��,�X,���H��,�V�X,���k�V����, ����,���t��,�s�쉖�l,���t��(�s�쉖�l-���D��),���D��,�������,��Y�a"),
	 _T("���Y�a,�������,���D��,���t��(�s�쉖�l-���D��),�s�쉖�l,���t��,����,���k�V����,�V�X,���H��,�X,�Ìy��,������,�C����,�،Ó�,�]����,�ܗŊs,���ِ�,�K��,�D����,�����̗�����"),
	 _T("�����̗�����,�D����,�K��,���ِ�,�ܗŊs,�]����,�،Ó�,�C����,������,�Ìy��,�X,���H��,�V�X,���k�V����, ����,���t��,�s�쉖�l,���t��(�s�쉖�l-���D��),���D��,������,�ю���"),
	 _T("�����̗�����,�D����,�K��,���ِ�,������,������,���m�[,��ΐ�,����(��),���ِ�,�c��"),
	 _T("�C�K,���C��,������,��������,�����q,���l��,���Óc"),
//	 _T(",,,,,,,,,"),
	_T(""),
	};
	TCHAR buffer[1024];
	int i;
	vector<RouteItem> replace_route;
	for (i = 0; '\0' != *route_def[i]; i++) {
		route.removeAll();
		replace_route.clear();
		_tcscpy_s<1024>(buffer, route_def[i]);
		setup_route(buffer);
		int cnt = Route::ReRouteRule86j87j(route.routeList(), &replace_route);
		show_route();
		_ftprintf(os, _T(">>>>>>>>>>>%d\n"), cnt);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		show_route();
		_ftprintf(os, _T("-----------\n\n"));
	}
	//


	_ftprintf(os, _T("\n/////////////// 86->69 //////////////////////////////////////////////////\n"));

	
	for (i = 0; '\0' != *route_def[i]; i++) {
		route.removeAll();
		_tcscpy_s<1024>(buffer, route_def[i]);
		setup_route(buffer);
		show_route();

		int cnt = Route::ReRouteRule86j87j(route.routeList(), &replace_route);
		_ftprintf(os, _T(">>>>>>>>>>>%d\n"), cnt);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		cnt = Route::ReRouteRule69j(route.routeList(), &replace_route);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		show_route();
		_ftprintf(os, _T("-----------\n\n"));
	}

	_ftprintf(os, _T("\n/////////////// 69 -> 86 //////////////////////////////////////////////////\n"));

	
	for (i = 0; '\0' != *route_def[i]; i++) {
		route.removeAll();
		_tcscpy_s<1024>(buffer, route_def[i]);
		setup_route(buffer);
		show_route();

		int cnt = Route::ReRouteRule69j(route.routeList(), &replace_route);
		_ftprintf(os, _T(">>>>>>>>>>>%d\n"), cnt);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		cnt = Route::ReRouteRule86j87j(route.routeList(), &replace_route);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		show_route();
		_ftprintf(os, _T("-----------\n\n"));
	}
}


/////////////////////////////////////////////////////////////////////////////////////

//	TEST Main
//	InitDlg����Ă΂��
//
int test_exec(void)
{
	fopen_s(&os, "test_result.txt", "w");

	_ftprintf(os, _T("\n---69---------------------------------------------------------\n"));
	test_a69();
	_ftprintf(os, _T("\n---86---------------------------------------------------------\n"));
	test_a86();

	fclose(os);
	return 1;
}


#endif





