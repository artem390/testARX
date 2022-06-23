#include <aced.h>
#include <geassign.h>
#include <rxregsvc.h>
#include <dbents.h>
#include <dbgroup.h>

void initApp();
void unloadApp();
void test();
void addObject(AcDbEntity* pEnt, AcDbObjectId id);
double getAngel(AcGeVector3d vector1, AcGeVector3d vector2);
AcGeVector3d getNormalVector(AcGeVector3d vector);
void initApp()
{
	acedRegCmds->addCommand(L"TEST_COMMANDS",
		L"TestARX",
		L"Test",
		ACRX_CMD_TRANSPARENT,
		test);
}
void unloadApp()
{
	acedRegCmds->removeGroup(L"TEST_COMMANDS");
}
void test()
{
	AcDbObjectId circleId, line1Id, line2Id;
	AcGePoint3d Ptc,Pt1,Pt2,Pt3;
	double R = 0;
	AcGeVector3d vector(0.0, 0.0, 1.0);
	AcString str;

	acedGetPoint(NULL, L"\ncenter: ", asDblArray(Ptc));
	acedGetDist(NULL, L"\nR: ", &R);	
	AcDbCircle* circle = new AcDbCircle(Ptc, vector, R);
	addObject(circle, circleId);
	circle->close();
	circleId = circle->id();

	acedGetPoint(NULL, L"\nPt1: ", asDblArray(Pt1));
	acedGetPoint(asDblArray(Pt1), L"\nPt2: ", asDblArray(Pt2));
	acedGetPoint(asDblArray(Pt2), L"\nPt3: ", asDblArray(Pt3));
	AcDbLine* line1 = new AcDbLine(Pt1, Pt2);
	AcDbLine* line2 = new AcDbLine(Pt2, Pt3);
	addObject(line1, line1Id);
	line1->close();
	addObject(line2, line2Id);
	line2->close();

	acedGetKword(L"\nPress Enter!", str);
	acdbOpenObject(circle, circleId, AcDb::kForWrite);
	
	AcGeVector3d vector1 = Pt1 - Pt2;
	AcGeVector3d vector2 = Pt3 - Pt2;

	double angle = getAngel(vector1, vector2);
	double l = R / sin(angle * 0.5);

	AcGeVector3d bisVector = getNormalVector(vector1) + getNormalVector(vector2);
	Ptc = Pt2 + (getNormalVector(bisVector)*l);
	circle->setCenter(Ptc);

	circle->close();
}

void addObject(AcDbEntity* pEnt, AcDbObjectId id)
{
	AcDbBlockTable* pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);
	AcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

	pBlockTable->close();

	pBlockTableRecord->appendAcDbEntity(id, pEnt);
	pBlockTableRecord->close();
}

double getAngel(AcGeVector3d vector1, AcGeVector3d vector2)
{
	double vector1Mod = sqrt(pow(vector1.x, 2) + pow(vector1.y, 2) + pow(vector1.z, 2));
	double vector2Mod = sqrt(pow(vector2.x, 2) + pow(vector2.y, 2) + pow(vector2.z, 2));
	return acos(((vector1.x * vector2.x) + (vector1.y * vector2.y) + (vector1.z * vector2.z)) / (vector1Mod * vector2Mod));
}

AcGeVector3d getNormalVector(AcGeVector3d vector)
{
	double length = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	return vector / length;
}

extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
	switch (msg)
	{
	case AcRx::kInitAppMsg:
		acrxDynamicLinker->unlockApplication(pkt);
		acrxRegisterAppMDIAware(pkt);
		initApp();
		break;
	case AcRx::kUnloadAppMsg:
		unloadApp();
		break;
	default:
		break;
	}
	return AcRx::kRetOK;
}