// WeaponMissileData.cpp: implementation of the CWeaponMissileData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeaponMissileData.h"
#include "AtumApplication.h"
#include "Cinema.h"	
#include "EnemyData.h"
#include "MonsterData.h"	
#include "SceneData.h"
#include "ShuttleChild.h"
#include "FieldWinSocket.h"
#include "ItemData.h"
//#include "ObjectRender.h"
#include "Background.h"
#include "ObjRender.h"
#include "dxutil.h"
#include "ItemInfo.h"
#include "StoreData.h" // 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - żŔĆŰ·ąŔĚĹÍ ±¸Çö	
#include "Interface.h" // 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - żŔĆŰ·ąŔĚĹÍ ±¸Çö	
#include "INFOpMain.h" // 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - żŔĆŰ·ąŔĚĹÍ ±¸Çö	
#include "INFOpInfo.h" // 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - żŔĆŰ·ąŔĚĹÍ ±¸Çö	
#include "INFTarget.h"
#include "D3DHanFont.h"
///////////////////////////////////////////////////////////////////////
///		˝şĹł °ü·Ă Á¤ŔÇą®
///		jschoi
///		2006.06.26
///////////////////////////////////////////////////////////////////////
constexpr auto no_hit_distance = 10000.0f;
#define TARGET_ON		1
#define TARGET_OFF		2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MISSILE_SPEED	100.0f
#ifdef _DEBUG
extern int		g_nMissileCount;
#endif
// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
// CWeaponMissileData::CWeaponMissileData(CAtumData * pAttack, 
// 									 ITEM * pWeaponITEM, 
// 									 ATTACK_DATA & attackData)

// CWeaponMissileData::CWeaponMissileData( CAtumData * pAttack,
// 									    ITEM * pWeaponITEM,
// 										ATTACK_DATA & attackData,
// 										ITEM* pEffectItem /* = NULL */ )
// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
CWeaponMissileData::CWeaponMissileData( CAtumData * pAttack,
									    ITEM * pWeaponITEM,
 										ATTACK_DATA & attackData,
										ITEM* pEffectItem, /* = NULL */
										int LoadingPriority )
//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
{
	// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
	m_LoadingPriority = LoadingPriority;
	//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
	m_pCharacterInfo = NULL;
	
	// 2005-08-16 by ispark
//	m_fWeaponLifeTime = 8.0f;
	m_fWeaponLifeTime = 0.0f;
	m_nSkillNum = attackData.AttackData.SkillNum;
	m_fExplosionRange = attackData.fExplosionRange;
	m_pTarget = (CAtumData*)g_pScene->FindUnitDataByClientIndex( attackData.AttackData.TargetInfo.TargetIndex );
	m_pAttacker = pAttack;
	m_nWeaponIndex = attackData.AttackData.WeaponIndex;
	m_nClientIndex = attackData.AttackData.AttackIndex;
	m_nTargetIndex = attackData.AttackData.TargetInfo.TargetIndex;
	m_nTargetItemFieldIndex = attackData.AttackData.TargetInfo.TargetItemFieldIndex;
	// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
	m_nMultiTargetIndex = attackData.AttackData.TargetInfo.MultiTargetIndex;
	// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
	m_bWeaponFlyType = attackData.bZigZagWeapon;
	m_vFirePos = A2DX(attackData.AttackData.FirePosition);
	m_pItemData = pWeaponITEM;

	// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
	m_pEffectItem = pEffectItem;
	// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

	// 2007-06-15 by dgwoo ŔÎĂľĆ®±îÁöµČ ą«±â ĽÓµµ.
	m_fWarheadSpeed = attackData.fWarheadSpeed;
//	if(pMsg->Distance - 128 >= 0)
//		m_bWeaponFlyType = 0;
//	else
//		m_bWeaponFlyType = 1;
	m_pCinema = NULL;
	SetShuttleChildOrderTarget();
	InitData();
//	g_pShuttleChild->m_fMissileFireTime += 0.1f;
//	m_fFireTime = g_pShuttleChild->m_fMissileFireTime;
//	if(m_fFireTime <= 0 && m_fFireTime > 1)
//	{
//		InitData();
//		m_fFireTime = 0;
//	}
	// 2005-11-25 by ispark, Áö»óĆř°ÝŔĚ¸é Ĺ¸°Ů Áß˝ÉżˇĽ­ ąÝ°ć 50żˇ ·Ł´ý
//	if(m_nSkillNum != 0)
	if(SKILL_BASE_NUM(m_nSkillNum) == BGEAR_SKILL_BASENUM_GROUNDBOMBINGMODE)
	{
		m_vTargetPos = A2DX(attackData.AttackData.TargetInfo.TargetPosition);
		m_vTargetPos.x += (float)(rand()%100 - 50);
		m_vTargetPos.z += (float)(rand()%100 - 50);
	}
	// 2006-12-01 by ispark, °řÁß Ćř°Ý
	if(SKILL_BASE_NUM(m_nSkillNum) == BGEAR_SKILL_BASENUM_AIRBOMBINGMODE)
	{
		m_vTargetPos = A2DX(attackData.AttackData.TargetInfo.TargetPosition);
		m_vTargetPos.x += (float)(rand()%100 - 50);
		m_vTargetPos.y += (float)(rand()%100 - 50);
		m_vTargetPos.z += (float)(rand()%100 - 50);
	}

	// 2005-07-19 by ispark
	// łŞ¸¦ ÇâÇŃ Ĺ¸°ŮŔĚ¶ó¸é Ĺ¸°ŮŔ» ŔúŔĺÇŃ´Ů. ŔĚČÄżˇ »čÁ¦˝Ă Č®ŔÎŔ» Ŕ§ÇŘĽ­ŔĚ´Ů. 
	if(attackData.AttackData.TargetInfo.TargetIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex)
	{
		m_nTargetMe = g_pShuttleChild->m_myShuttleInfo.ClientIndex;
		g_pShuttleChild->SetMissileWarning(TRUE);
		g_pShuttleChild->SetMissileCount(g_pShuttleChild->GetMissileCount() + 1);

		// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - żŔĆŰ·ąŔĚĹÍ ±¸Çö	
		if( g_pStoreData->FindItemInInventoryByWindowPos( POS_PET ) )
		{
			g_pInterface->m_pINFOpMain->GetOpInfo()->SetOperatorAction(1,2);
		}
		// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - żŔĆŰ·ąŔĚĹÍ ±¸Çö

	}


	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	m_nDelegateClientIdx	= attackData.AttackData.DelegateClientIdx;
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

#ifdef _DEBUG
	g_nMissileCount = g_pShuttleChild->GetMissileCount();
#endif
}


/*
CWeaponMissileData::CWeaponMissileData(CAtumData * pAttack,
									   CAtumData * pTarget,
									   MSG_FC_BATTLE_ATTACK_RESULT_SECONDARY* pMsg)//float fDist,int nWeaponIndex, int nClientIndex,D3DXVECTOR3 vTargetPos);
{
	FLOG( "CWeaponMissileData(CAtumData * pAttack,CAtumData * pTarget,MSG_FC_BATTLE_ATTACK_RESULT_SECONDARY* pMsg)" );
	m_pCharacterInfo = NULL;
	
	m_fWeaponLifeTime = 8.0f;
	m_pTarget = pTarget;
	m_pAttacker = pAttack;
	m_nWeaponIndex = pMsg->WeaponIndex;
	m_nClientIndex = pMsg->AttackIndex;
	m_nTargetIndex = pMsg->TargetIndex;
	m_nTargetItemFieldIndex = -1;
	if(pMsg->Distance - 128 >= 0)
		m_bWeaponFlyType = 0;
	else
		m_bWeaponFlyType = 1;
	m_pCinema = NULL;
	SetShuttleChildOrderTarget();
}

CWeaponMissileData::CWeaponMissileData(CAtumData * pAttack,
									   CAtumData * pTarget,
									   MSG_FC_BATTLE_ATTACK_ITEM_RESULT_SECONDARY* pMsg)//float fDist,int nWeaponIndex, int nClientIndex,D3DXVECTOR3 vTargetPos);
{
	FLOG( "CWeaponMissileData(CAtumData * pAttack,CAtumData * pTarget,MSG_FC_BATTLE_ATTACK_ITEM_RESULT_SECONDARY* pMsg)" );
	m_pCharacterInfo = NULL;
	
	m_fWeaponLifeTime = 8.0f;
	m_pTarget = pTarget;
	m_pAttacker = pAttack;
	m_nWeaponIndex = pMsg->WeaponIndex;
	m_nClientIndex = pMsg->AttackIndex;
	m_nTargetIndex = pMsg->TargetIndex;
	m_nTargetItemFieldIndex = pMsg->TargetItemFieldIndex;
	if(pMsg->Distance - 128 >= 0)
		m_bWeaponFlyType = 0;
	else
		m_bWeaponFlyType = 1;
	SetShuttleChildOrderTarget();
}
*/
CWeaponMissileData::~CWeaponMissileData()
{
	FLOG( "~CWeaponMissileData()" );

	if(m_pCharacterInfo)//Ăß°ˇ
	{
		m_pCharacterInfo->InvalidateDeviceObjects();
		m_pCharacterInfo->DeleteDeviceObjects();
		SAFE_DELETE(m_pCharacterInfo);
	}
	SAFE_DELETE( m_pCinema );

	// 2005-07-11 by ispark
	// ąĚ»çŔĎ °ć°í 
	// ąĚ»çŔĎ °ąĽö¸¦ ÇĎłŞľż »čÁ¦¸¦ ÇŃ´Ů.
	// ¸¸ľŕżˇ °ąĽö°ˇ ¸¶ŔĚłĘ˝ş¶ó¸é 0Ŕ¸·Î ĂĘ±âČ­ ÇŃ´Ů.
	// 1ąř ĆÄ±« ¸í·ÉľřŔĚ »ç¶óÁö´Â °ćżě´Ů.
	CheckTargetWarning();

#ifdef _DEBUG
	if(g_pShuttleChild)
		g_nMissileCount = g_pShuttleChild->GetMissileCount();
#endif

}

void CWeaponMissileData::InitData()
{
	FLOG( "CWeaponMissileData::InitData()" );
	if(!m_pItemData)
	{
		m_bUsing = FALSE;
		return;
	}

	D3DXVECTOR3 vVel = m_pAttacker->m_vVel;
	m_vUp = m_pAttacker->m_vUp;
	D3DXVECTOR3 vSide;
	D3DXVec3Cross(&vSide,&vVel,&m_vUp);
	D3DXVec3Normalize(&vSide,&vSide);

	if(m_bWeaponFlyType == 0)
	{
		m_vPos = m_pAttacker->m_vLWSecondaryPos + m_vFirePos.x*vSide/4;
//		m_vPos = m_pAttacker->m_vLWSecondaryPos + m_vFirePos.x*vSide;
	}
	else
	{
		m_vPos = m_pAttacker->m_vRWSecondaryPos + m_vFirePos.x*vSide/4;
//		m_vPos = m_pAttacker->m_vRWSecondaryPos + m_vFirePos.x*vSide;
	}

	m_vVel = m_pAttacker->m_vVel;
	m_vOriPos = m_vPos;
	D3DXVec3Normalize(&m_vVel,&m_vVel);
	m_dwWeaponState = _NORMAL;
	m_fWeaponSpeed = MISSILE_SPEED;
	m_bSetTarget = FALSE;

	D3DXMatrixLookAtLH( &m_mMatrix, &(m_vPos), &(m_vPos + m_vVel), &m_vUp);
	D3DXMatrixInverse( &m_mMatrix, NULL, &m_mMatrix );
	char buf[256];
	memset(buf,0x00,sizeof(buf));
	
	// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
	//wsprintf(buf,"%08d",m_pItemData->SourceIndex);								// 2005-08-23 by ispark

	// ŔĚĆĺĆ® ľĆŔĚĹŰŔĚ ŔÖ´Ů¸é ±×°ÍŔ¸·Î »ýĽş
	if( m_pEffectItem )
		wsprintf( buf, "%08d", m_pEffectItem->SourceIndex );
	else
		wsprintf( buf, "%08d", m_pItemData->SourceIndex );
	
	// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

	LoadCharacterEffect(buf);
//	m_bodyCondition = BODYCON_BULLET_MASK;
	if(m_pCharacterInfo)
	{
		// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
		m_pCharacterInfo->m_LoadingPriority = m_LoadingPriority;
		//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
		m_pCharacterInfo->SetAllBodyConditionMatrix(m_mMatrix);
//		m_pCharacterInfo->ChangeBodyCondition(m_bodyCondition);
		m_pCharacterInfo->ChangeBodyCondition(BODYCON_BULLET_MASK);
		// 2004-10-12 by jschoi
		if(m_nSkillNum != 0)
		{
			ChangeBodyConditionForSkillEffect(m_nSkillNum,BODYCON_BULLET_MASK);
		}
	}
	else
	{
		// temporary item
		LoadCharacterEffect("01200016");
		if(m_pCharacterInfo)
		{
			// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
			m_pCharacterInfo->m_LoadingPriority = m_LoadingPriority;
			//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
			m_pCharacterInfo->SetAllBodyConditionMatrix(m_mMatrix);
			m_pCharacterInfo->ChangeBodyCondition(BODYCON_BULLET_MASK);
			if(m_nSkillNum != 0)
			{
				ChangeBodyConditionForSkillEffect(m_nSkillNum,BODYCON_BULLET_MASK);
			}
		}
		else
		{
			g_pD3dApp->NetworkErrorMsgBox(STRMSG_C_050512_0001);
			return;
		}		
	}
	m_dwPartType = _MISSILE;

//  ˝Ăł×¸¶ Ć÷ŔÎĆ® ĽÂĆĂ/////////////////////////////////////////////////////////////////
	char str[32];
//	sprintf(str,"%08d",m_pItemData->ItemNum);
	sprintf(str,"%08d",m_pItemData->CameraPattern);
	SAFE_DELETE(m_pCinema);
	m_pCinema = g_pScene->LoadCinemaData(str, m_pItemData->CameraPattern);
	if(m_pCinema == NULL)
	{
		#ifdef _DEBUG
			DBGOUT( "Cinema : Can't Find Pattern(Cinema) Files(%s)\n",str);
		#endif //_DEBUG_endif
//		m_pCinema = g_pScene->LoadCinemaData(PATTERN_DEFAULT);
		if(m_pCinema == NULL)
		{
			g_pD3dApp->NetworkErrorMsgBox(STRMSG_C_050513_0001);
			return;
		}
	}
	EVENT_POINT ep;
	ep.vPosition = m_vPos;
//	ep.vDirection = m_vVel;
//	ep.vTarget = m_vVel;

	// 2005-11-25 by ispark, Ćř°ÝŔĚ¸é Ĺ¸°ŮŔĚ ľř´Ů. ±×·ŻłŞ Ĺ¸°ŮŔş ąŮ´ÚŔĚąÇ·Î ·Ł´ýŔ¸·Î łŞ°ˇ°Ô ÇŃ´Ů. ČÎ ¸ÚŔÖ´Ů.
	if(m_pTarget || m_nSkillNum != 0)
	{
		D3DXVECTOR3 vRandomVector;
		vRandomVector.x = ((float)(rand()%50 - 25))/100;
		vRandomVector.y = ((float)(rand()%50 - 25))/100;
		vRandomVector.z = ((float)(rand()%50 - 25))/100;
		ep.vDirection = m_vVel + vRandomVector;
		ep.vTarget = m_vVel + vRandomVector;
	}
	else
	{
		ep.vDirection = m_vVel;
		ep.vTarget = m_vVel;
	}
	ep.vUpVector = m_vUp;
	D3DXVec3Normalize(&ep.vDirection,&ep.vDirection);
	ep.fVelocity = m_fWeaponSpeed;	// m_fWeaponSpeed ĂĘ±â ˝şÇÇµĺ
	ep.fCurvature = DEFAULT_CURVATURE;	// ĂĘ±â°Ş ĽÂĆĂ
//	m_pCinema->InitCinemaData(ep,m_bWeaponFlyType == 0);

	// 2007-06-15 by dgwoo ľĆ·ˇ ĽŇ˝ş·Î şŻ°ć.
//	m_pCinema->InitWeaponCinemaData(ep,m_bWeaponFlyType == 0, m_pItemData->RepeatTime, m_pItemData->BoosterAngle);
	m_pCinema->InitWeaponCinemaData(ep,m_bWeaponFlyType == 0, m_fWarheadSpeed, m_pItemData->BoosterAngle);
//  ĂĘ±âČ­ Áľ·á ///////////////////////////////////////////////////////////////////////
}

void CWeaponMissileData::Tick()
{
	FLOG( "CWeaponMissileData::Tick()" );
	float fElapsedTime = g_pD3dApp->GetElapsedTime();
//	if(m_fFireTime > 0)
//	{
//		m_fFireTime -= fElapsedTime;
//		if(m_fFireTime <= 0)
//		{
//			InitData();
//			m_fFireTime = 0;
//		}
//		return;
//	}
	if(m_pAttacker == NULL)
	{
		m_bUsing = FALSE;
		return;
	}

// 2005-11-25 by ispark, Áö»óĆř°Ý, °řÁßĆř°Ý
	if(m_nSkillNum != 0)
	{
		SkillTick();
		// SKILL EFFECT
		SetBodyConditionMatrixForSkillEffect(m_bodyCondition,m_mMatrix);
		return;
	}

	m_vOldPos = m_vPos;
//	D3DXVECTOR3 vVel;
//	D3DXVECTOR3 vSide,vUpTemp;
//	D3DXMATRIX mat;
//	D3DXVECTOR3 vUp(0,1,0);

	// ĆĐĹĎ Ĺ¸ÄĎąćÇâ °ü·Ă 2004-07-27 jschoi
	int nPatternType = m_pCinema->GetHeader().nPatternType;
	D3DXVECTOR3 vTargetPosition;

	if( m_dwWeaponState == _EXPLODING ) // exploding state
	{
		if(m_pTarget)
		{
			if(!((CUnitData*)m_pTarget)->m_bShielding)
			{
				// ·Ł´ýÇĎ°Ô Ŕ§Äˇ¸¦ Á¶Á¤ÇŃ´Ů.
				D3DXVECTOR3 vVel;
				D3DXVec3Normalize(&vVel, &(m_pAttacker->m_vPos - m_pTarget->m_vPos));
				vVel = D3DXVECTOR3( vVel.x*((float)(rand()%5)),vVel.y*((float)(rand()%5)),vVel.z*((float)(rand()%5)));
				m_vPos += vVel;
			}
			else	// ˝Żµĺ ąßµż ÁßŔÎ°ćżě ÁÂÇĄ´Â ˝ŻµĺĹ©±â ¸¸Ĺ­Ŕ¸·Î Á¶Á¤ÇŃ´Ů.
			{// ˝Żµĺ ąßµż ÁßŔĎ¶§ ˝Żµĺ Hit ŔĚĆĺĆ® Ăß°ˇ
				D3DXVECTOR3 vVel;
				D3DXVec3Normalize(&vVel, &(m_vPos - m_pTarget->m_vPos));
				m_vPos += vVel*SIZE_OF_SHIELD_EFFECT;
				((CUnitData*)m_pTarget)->CreateSecondaryShieldDamage(m_pAttacker->m_vPos);
			}

			// 2005-07-11 by ispark
			// ąĚ»çŔĎ °ć°í 
			// ąĚ»çŔĎ °ąĽö¸¦ ÇĎłŞľż »čÁ¦¸¦ ÇŃ´Ů.
			// ¸¸ľŕżˇ °ąĽö°ˇ ¸¶ŔĚłĘ˝ş¶ó¸é 0Ŕ¸·Î ĂĘ±âČ­ ÇŃ´Ů.
			// 2ąř ĆÄ±« ¸í·ÉŔĚ ł»·ÁÁö´Â °ćżě´Ů.
			CheckTargetWarning();
		}
		SetBodyCondition(BODYCON_HIT_MASK);
		if(m_nSkillNum != 0)
		{
			ChangeBodyConditionForSkillEffect(m_nSkillNum,BODYCON_HIT_MASK);
		}

		m_dwWeaponState = _EXPLODED;
	} 
	else if(m_dwWeaponState == _NORMAL)
	{
		CItemData *pTargetItem = g_pScene->FindFieldItemByFieldIndex( m_nTargetItemFieldIndex );
//		if(!pTargetItem && m_pTarget)
//		{
//			pTargetItem = g_pScene->FindFieldItemByParent( m_pTarget );
//		}
		m_fWeaponLifeCheckTime += fElapsedTime;
		if(m_fWeaponLifeCheckTime > 0.5f)
			m_bSetTarget = TRUE;

		// 2006-10-19 by ispark, Á×ľúŔ» ¶§ Ĺ¸°Ů °ć°í ľřľÖ±â
		if(m_pTarget == NULL ||
			(g_pD3dApp->m_pShuttleChild == m_pTarget && g_pShuttleChild->CheckUnitState() == BREAKDOWN))
		{
			CheckTargetWarning(); // Ĺ¸°ŮŔĚ ľřľîÁö¸é °ć°í ¸ŢĽĽÁö »čÁ¦
		}
		// 2005-08-16 by ispark
//		if(m_fWeaponLifeTime <= m_fWeaponLifeCheckTime)
		// ¸ó˝şĹÍ°ˇ ˝đ ąĚ»çŔĎŔĚ¸é, »ç°Ĺ¸®¸¦ °­Á¦·Î Áő°ˇ
		float fMonWeapon = 0.0f;
		if(IS_SECONDARY_WEAPON_MONSTER(m_pItemData->Kind))
		{
			fMonWeapon = m_pItemData->Range * 2.0f;
		}
		
		if(m_fWeaponLifeCheckTime * m_fWeaponSpeed > m_pItemData->Range + fMonWeapon)
		{
			m_dwWeaponState = _EXPLODING;
			m_pTarget = NULL;
		}
		else
		{
			CheckTargetState();
			if(pTargetItem && pTargetItem->m_dwState == _NORMAL)
			{
				m_vTargetPos = pTargetItem->m_vPos;
			}
			else if(m_pTarget && ATTACK_AVAILABLE_STATE(m_pTarget->m_dwState))
			{
				// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
				//m_vTargetPos = m_pTarget->m_vPos;
				// 2011. 09. 28 by jskim łÍ Ĺ¸°Ů ˝Ă˝şĹŰ ąö±× ĽöÁ¤( ÄÉ˝şĆĂ ą®Á¦ )
				//if(m_pTarget && ((CMonsterData*)m_pTarget)->m_vecvmultiData.size() > 0)
				if( m_pTarget && 
					m_pTarget->m_dwPartType == _MONSTER &&
					((CMonsterData*)m_pTarget)->m_vecvmultiData.size() > 0)
				// end 2011. 09. 28 by jskim łÍ Ĺ¸°Ů ˝Ă˝şĹŰ ąö±× ĽöÁ¤( ÄÉ˝şĆĂ ą®Á¦ )
				{
					m_vTargetPos = ((CMonsterData*)m_pTarget)->GetMultiPos( m_nMultiTargetIndex );
				}
				else if(m_pTarget)
				{
					m_vTargetPos = m_pTarget->m_vPos;
				}
				// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ				
			}
			else
			{
				m_vTargetPos = D3DXVECTOR3(0,0,0);
				m_bSetTarget = FALSE;
			}

//			DBGOUT("%d. Ĺ¸°Ů(%.0f, %.0f, %.0f) ", m_nWeaponIndex, m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z);
//			DBGOUT("ąĚ»çŔĎ(%.0f, %.0f, %.0f) ", m_vPos.x, m_vPos.y, m_vPos.z);
//			DBGOUT("°Ĺ¸® = %.0f\n", D3DXVec3Length(&(m_vPos - m_vTargetPos)));
			// MoveWeapon ´ë˝Ĺ ĆĐĹĎ Ŕűżë
			BOOL bResult;							// ¸ńÇĄÁˇżˇ µµ´ŢÇß´ÂÁö °á°ú..
//			bResult=m_pCinema->Tick(m_vTargetPos);	// °á°ú°ˇ FALSE ¸é ¸ńÇĄ¸¦ Áöłµ´Ů.. Áď µµ´ŢÇß´Ů.


			// 2010. 03. 18 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(¸ó˝şĹÍ ˝şĹł Ăß°ˇ)

			//bResult=m_pCinema->WeaponTick(m_vTargetPos, fElapsedTime);	// °á°ú°ˇ FALSE ¸é ¸ńÇĄ¸¦ Áöłµ´Ů.. Áď µµ´ŢÇß´Ů.
			float fDecSpeed = 0.0f;
			if( m_pTarget && m_pTarget->m_dwPartType == _MONSTER )
			{
				ITEM* pSkillItem = ((CMonsterData*)(m_pTarget))->GetSkillItemWithDesParam( DES_BLIND );
				if( !pSkillItem || pSkillItem->Range < D3DXVec3Length( &(m_vPos - m_pTarget->m_vPos) ) )
					m_nBlindCumulate = m_nBlindSpeedDownTime = 0;
				else
				{
					DWORD nCurTime	= timeGetTime();
					if( nCurTime > m_nBlindSpeedDownTime + BLIND_INTERVAL )
					{
						m_nBlindCumulate++;
						m_nBlindSpeedDownTime = nCurTime;
					}

					if( m_nBlindCumulate )
						fDecSpeed = m_nBlindCumulate * pSkillItem->GetParameterValue( DES_BLIND );
				}
			}

			bResult=m_pCinema->WeaponTick( m_vTargetPos, fElapsedTime, fDecSpeed );

			// end 2010. 03. 18 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(¸ó˝şĹÍ ˝şĹł Ăß°ˇ)


			EVENT_POINT ep;
			ep=m_pCinema->GetCurrentCinemaPoint();
			m_vPos = ep.vPosition;			// Ŕ§Äˇ
			m_vVel = ep.vDirection;			// ąćÇâ
			m_fWeaponSpeed = ep.fVelocity;	// ĽÓ·Â
			//DBGOUT("ąĚ»çŔĎ ĽÓµµ : %3.f\n",m_fWeaponSpeed);
			m_vUp = ep.vUpVector;
			vTargetPosition = ep.vTarget;
			if(bResult == FALSE)
			{
				nPatternType = TARGET_OFF;
			}

			// ł»°ˇ ąß»çÇŃ ą«±âŔĚ°ĹłŞ ¸ó˝şĹÍ°ˇ ąß»çÇŃ 2Çü ą«±âŔĎ¶§ Ăćµą ĂĽĹ©
			if(	m_nClientIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex ||

				// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
				// ¸ó˝şĹÍ °ř°ÝŔĚ¸éĽ­ ¸ó˝şĹÍ°ˇ Ĺ¸°ŮŔĚ°í ł»°ˇ Ŕ§ŔÓąŢľŇ´Ů¸é ł»°ˇ Ăćµą Ăł¸®¸¦ ÇŃ´Ů
				( IS_SECONDARY_WEAPON_MONSTER(m_pItemData->Kind) &&
				  IS_MONSTER_CLIENT_INDEX( m_nTargetIndex ) &&
				  (m_nDelegateClientIdx == g_pShuttleChild->m_myShuttleInfo.ClientIndex) ) ||
				// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

				(IS_SECONDARY_WEAPON_MONSTER(m_pItemData->Kind) &&
				m_nTargetIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex ))
			{
				float fMovingDistance = ep.fVelocity*fElapsedTime;
				CheckWeaponCollision(pTargetItem,fMovingDistance);

				// 2006-12-08 by ispark, ·ąŔĚ´ő »ç°Ĺ¸®¸¦ ąţľîłŞ¸é Ĺ¸°ŮŔ» ľřľÚ
				// ·ąŔĚ´ő »ç°Ĺ¸®ŔÇ 1.5ąč
				//float fRadarRange = (float)g_pShuttleChild->m_pRadarItemInfo->ItemInfo->Range * 1.5f;
				if(m_pAttacker == g_pShuttleChild)
				{
					if(g_pShuttleChild->m_pRadarItemInfo)
					{
						float fRadarRange = CAtumSJ::GetSecondaryRadarRange(g_pShuttleChild->m_pRadarItemInfo->ItemInfo, &g_pShuttleChild->m_paramFactor) * 1.5f;
						if(D3DXVec3Length(&(m_pAttacker->m_vPos - m_vPos)) > fRadarRange)
						{ 
							m_pTarget = NULL;
						}
					}
					
				}
			}
			else if(m_nTargetIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex)
			{
				// ł»°ˇ Ĺ¸°ŮŔĚ°í ł» Č­¸éżˇ ş¸ż©ÇŇ şÎşĐ
				if(m_pTarget)
				{
				D3DXVECTOR3 vTargetVel = m_pTarget->m_vPos - m_vPos;
					float fMovingDistance = m_fWeaponSpeed * fElapsedTime;

					if(D3DXVec3Length(&vTargetVel) < m_pTarget->m_fObjectSize + fMovingDistance + 5.0f &&
						m_bEvasion)
					{
						m_pTarget = NULL;
					}
				}
			}
			// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
			else if( m_nTargetIndex != g_pShuttleChild->m_myShuttleInfo.ClientIndex )
			{
				// ´Ů¸Ą ŔŻŔú°ˇ ÇÇÇŃ°Í Ĺ¸°Ů ľřľÖÁŘ´Ů
				if( m_bEvasion )
					m_pTarget = NULL;
			}
			// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
		}
	}
	else if( m_dwWeaponState == _EXPLODED )
	{
		if(m_pCharacterInfo)
		{
			if(!m_pCharacterInfo->IsUsing())
				m_bUsing = FALSE;
		}
		else
		{
			m_bUsing = FALSE;
		}
	}
	D3DXVECTOR3 vSide;
	D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
	D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);

	// ĆĐĹĎ Ĺ¸ÄĎ ąćÇâ °ü·Ă 2004-07-27 jschoi
	if(nPatternType == TARGET_OFF)
	{
		D3DXMatrixLookAtRH( &m_mMatrix, &(m_vPos), &(m_vPos + m_vVel), &m_vUp);
	}
	else
	{
		D3DXMatrixLookAtRH( &m_mMatrix, &(m_vPos), &(vTargetPosition), &m_vUp);
	}

	D3DXMatrixInverse( &m_mMatrix, NULL, &m_mMatrix );
	// effect matrix & ticking
	if(m_pCharacterInfo)
	{
		m_pCharacterInfo->SetAllBodyConditionMatrix(m_mMatrix );
//		m_pCharacterInfo->SetSingleBodyConditionMatrix( BODYCON_FIRE_MASK,m_mFireMatrix );
		m_pCharacterInfo->Tick(fElapsedTime);
	}
	
	// SKILL EFFECT
	SetBodyConditionMatrixForSkillEffect(m_bodyCondition,m_mMatrix);
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			CWeaponMissileData::CheckWeaponCollision()
/// \brief		żţĆů ĂćµąĂł¸®
/// \author		dhkwon
/// \date		2004-05-28 ~ 2004-05-28
/// \warning	Â÷ČÄżˇ ÇĎłŞ·Î ą­Ŕ» żąÁ¤
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CWeaponMissileData::CheckWeaponCollision(CItemData *pTargetItem,float fMovingDistance)
{
	int nTargetIndex = m_nTargetIndex;
//	BOOL bAttackAvailable = TRUE;// °ř°Ý°ˇ´ÉÇŃ ŔŻ´ÖŔÎÁö °Ë»ç..(ŔĎąÝ ŔŻŔú°ř°Ý ¸·±â)
	DWORD dwTargetState = _NORMAL; // °ř°Ý°ˇ´ÉÇŃ »óĹÂŔÎÁö ĆÄľÇ..(ŔűŔÇ »óĹÂ°ˇ DEADŔĚ¸é °ř°ÝÇĎÁö ľĘŔ˝)
	CMonsterData*	pMonster = NULL;
	CEnemyData *pEnemy = NULL;		// 2005-07-07 by ispark	// ·Ń¸µ˝Ă˝şĹŰŔ» »çżë˝Ă¸¦ Ŕ§ÇŘĽ­

	// Ĺ¸°Ů ŔŻ´Ö ±¸şĐ
	if(m_nTargetIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex) // ShuttleChild : Monster ==> ShuttleChild
	{
//		bAttackAvailable = TRUE;
		dwTargetState = g_pShuttleChild->m_dwState;
	}
	else if( m_pTarget && m_pTarget->m_dwPartType == _ENEMY) // ENEMY : ShuttleChild ==> Enemy
	{
		CMapEnemyIterator itEnemy = 
			g_pScene->m_mapEnemyList.find(((CEnemyData *)m_pTarget)->m_infoCharacter.CharacterInfo.ClientIndex);
		if(itEnemy != g_pScene->m_mapEnemyList.end())
		{
			pEnemy = itEnemy->second;		// 2005-07-07 by ispark
//			bAttackAvailable = itEnemy->second->m_bAttackEnemy;
			dwTargetState = itEnemy->second->m_dwState;
		}
		else 
		{
			m_pTarget = NULL;
			return;
		}
	}
	else if( m_pTarget && m_pTarget->m_dwPartType == _MONSTER)	// MONSTER : ShuttleChild ==> Monster
	{
		CMapMonsterIterator itMonster = g_pScene->m_mapMonsterList.find(((CMonsterData *)m_pTarget)->m_info.MonsterIndex);
		if(itMonster != g_pScene->m_mapMonsterList.end())
		{
//			bAttackAvailable = TRUE;
			dwTargetState = itMonster->second->m_dwState;
			pMonster = itMonster->second;
		}
		else
		{
			m_pTarget = NULL;
			return;
		}
	}

//	if(bAttackAvailable && ATTACK_AVAILABLE_STATE(dwTargetState))
	if(ATTACK_AVAILABLE_STATE(dwTargetState))
	{
		BOOL bCollision = FALSE;
		float fMissileSize = 5.0f;
		fMovingDistance = fMovingDistance < 10.0f ? 10.0f : fMovingDistance;
		D3DXVECTOR3 vSide,vUpTemp;
		D3DXMATRIX mat;
		D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
		D3DXVec3Cross(&vUpTemp,&m_vVel,&vSide);
		D3DXMatrixLookAtLH( &mat, &m_vPos, &(m_vPos+m_vVel), &vUpTemp);
		float fDist;

		// Ăćµą °Ë»ç
		if( m_pTarget )
		{
			D3DXVECTOR3 vTargetVel = m_pTarget->m_vPos - m_vPos;

			if(pMonster == NULL || pMonster->m_pMonMesh == NULL )
			{
				if(D3DXVec3Length(&vTargetVel) < m_pTarget->m_fObjectSize + fMissileSize + fMovingDistance)
//				if(D3DXVec3Length(&vTargetVel) < (m_pTarget->m_fObjectSize / 2) + fMissileSize)
				{
					// 2005-07-07 by ispark
					// ·Ń¸µ ˝Ă˝şĹŰ
					bCollision = RollingCollision(pEnemy);
					if(bCollision == FALSE)
					{
						SendFieldSocketBattleAttackEvasion(nTargetIndex, pTargetItem==NULL ? 0:pTargetItem->m_nItemIndex,m_nClientIndex,m_pItemData->ItemNum);
					}
				}				
			}
			else
			{
				float fMonsterRadius = pMonster->m_pMonMesh->m_fRadius;

				if(fMonsterRadius > 50 && pMonster->m_pMonsterInfo->MonsterUnitKind != 2011500)
					fMonsterRadius = 50;
	
				if( m_pTarget->m_dwPartType == _MONSTER && fMonsterRadius > BIG_MONSTER_SIZE) //wont be used anymore due to rockets rolling around monsters (tarts etc)
				{
					if(D3DXVec3Length(&vTargetVel) < fMonsterRadius + fMissileSize + fMovingDistance)
					{
						pMonster->m_pMonMesh->Tick(pMonster->m_fCurrentTime);
						pMonster->m_pMonMesh->SetWorldMatrix(pMonster->m_mMatrix);
						float fcollDist = pMonster->m_pMonMesh->CheckCollision(mat, m_vPos, fMovingDistance, TRUE, FALSE).fDist;
						if(fcollDist < fMovingDistance )
						{
							m_vPos += m_vVel * fcollDist;
							bCollision = TRUE;
						}
					}		
				}
				else	// ĽŇÇü ¸ó˝şĹÍ
				{
//					if(D3DXVec3Length(&vTargetVel) < m_pTarget->m_fObjectSize + fMissileSize + fMovingDistance)
					if(D3DXVec3Length(&vTargetVel) < fMonsterRadius + fMissileSize)
					{
						bCollision = TRUE;
					}
				}
			}
		}
		//Áö»ó Ăćµą
		if(!bCollision)
		{
			fDist = g_pScene->m_pObjectRender->CheckCollMeshRangeObject(mat,m_vPos,fMovingDistance).fDist;
			if(fDist < fMovingDistance)
			{
				m_vPos += m_vVel * fDist;
				nTargetIndex = 0;
				bCollision = TRUE;
				m_pTarget = NULL;
			}
			if(!bCollision &&
				IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)) // 2005-12-07 by ispark, ąŮ´ÚŔ» ·»´ő¸µŔ» ÇĎ¸é Ăćµą °Ë»ç
			{
				// ąŮ´Ú Ăćµą
				float fHeight = g_pGround->CheckHeightMap(m_vPos + m_vVel*fMovingDistance);
				if(fHeight > m_vPos.y)
				{
					nTargetIndex = 0;
					m_pTarget = NULL;
					bCollision = TRUE;
				}				
			}
		}

		if(bCollision)
		{
			SendFieldSocketBattleAttackFind(nTargetIndex, pTargetItem==NULL ? 0:pTargetItem->m_nItemIndex,m_nClientIndex,m_pItemData->ItemNum);
			// ÇÁ·ÎĹäÄÝ ŔüĽŰ
/*			if(m_nClientIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex)// Ĺ¬¶óŔĚľđĆ®(CShuttleChild)
			{
				if(pTargetItem)
				{
//					SendFieldSocketBattleAttackItemFind(nTargetIndex, pTargetItem->m_nItemIndex);
					SendFieldSocketBattleAttackFind(nTargetIndex, pTargetItem==NULL ? 0:pTargetItem->m_nItemIndex,m_nClientIndex,m_pItemData->ItemNum);
				}
				else
				{
//					SendFieldSocketBattleAttackFind(nTargetIndex, 0);
					SendFieldSocketBattleAttackFind(nTargetIndex, 0 ,m_nClientIndex , m_pItemData->ItemNum);
				}
			}
			else // ¸ó˝şĹÍ 2Çü ą«±âŔÎ °ćżě
			{
				if(pTargetItem)
				{
//					SendFieldSocketBattleMonsterAttackItemFind(nTargetIndex, pTargetItem->m_nItemIndex);
					SendFieldSocketBattleAttackFind(nTargetIndex, pTargetItem->m_nItemIndex,m_nClientIndex,m_pItemData->ItemNum);
				}
				else
				{
//					SendFieldSocketBattleMonsterAttackFind(nTargetIndex);
					SendFieldSocketBattleAttackFind(nTargetIndex, 0 ,m_nClientIndex , m_pItemData->ItemNum);
				}
			}
*/			//m_pTarget = NULL;
			m_dwWeaponState = _EXPLODING;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CWeaponMissileData::SkillTick()
/// \brief		ąĚ»çŔĎ Áö»óĆř°Ý
/// \author		ispark
/// \date		2005-11-25 ~ 2005-11-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CWeaponMissileData::SkillTick()
{
	float fElapsedTime = g_pD3dApp->GetElapsedTime();
	m_vOldPos = m_vPos;
//	D3DXVECTOR3 vSide,vUpTemp;
//	D3DXMATRIX mat;
//	D3DXVECTOR3 vUp(0,1,0);

	m_fTargetDistance = D3DXVec3Length(&(m_vStartPos - (m_pTarget ? m_pTarget->m_vPos : m_vTargetPos)));
	
	// ĆĐĹĎ Ĺ¸ÄĎąćÇâ °ü·Ă 2004-07-27 jschoi
	int nPatternType = m_pCinema->GetHeader().nPatternType;
	D3DXVECTOR3 vTargetPosition;

	if(m_dwWeaponState == _EXPLODING) // exploding state
	{
		if(m_pTarget)
		{
			if(((CUnitData*)m_pTarget)->m_bShielding == TRUE && m_pAttacker)
			{// ˝Żµĺ ąßµż ÁßŔĎ¶§ ˝Żµĺ Hit ŔĚĆĺĆ® Ăß°ˇ
				//D3DXVECTOR3 vVel;
				//D3DXVec3Normalize(&vVel, &(m_pAttacker->m_vPos - m_pTarget->m_vPos));
				//m_vPos = m_pTarget->m_vPos + vVel*SIZE_OF_SHIELD_EFFECT;
				((CUnitData*)m_pTarget)->CreateSecondaryShieldDamage(m_pAttacker->m_vPos);
			}
			else	
			{
				// ·Ł´ýÇĎ°Ô Ŕ§Äˇ¸¦ Á¶Á¤ÇŃ´Ů.
				//D3DXVECTOR3 vVel;
				//D3DXVec3Normalize(&vVel, &(m_pAttacker->m_vPos - m_pTarget->m_vPos));
				//vVel = D3DXVECTOR3( vVel.x*((float)(rand()%5)),vVel.y*((float)(rand()%5)),vVel.z*((float)(rand()%5)));
				//m_vPos = m_pTarget->m_vPos + vVel;
			}
		}
		//Ćřąß ŔĚĆĺĆ®
		SetBodyCondition(BODYCON_HIT_MASK);
		if(m_nSkillNum != 0)
		{
			ChangeBodyConditionForSkillEffect(m_nSkillNum,BODYCON_HIT_MASK);
		}	
		m_dwWeaponState = _EXPLODED;
		
	} 
	else if(m_dwWeaponState == _NORMAL)
	{
		CItemData *pTargetItem = g_pScene->FindFieldItemByFieldIndex( m_nTargetItemFieldIndex );
		m_fWeaponLifeCheckTime += fElapsedTime;
		if(m_fWeaponLifeCheckTime > 0.5f)
			m_bSetTarget = TRUE;
		if(m_fWeaponLifeCheckTime * m_fWeaponSpeed > m_pItemData->Range)
		{
			m_dwWeaponState = _EXPLODING;
			m_pTarget = NULL;
		}
		else
		{
			CheckTargetState();
			if(pTargetItem && pTargetItem->m_dwState == _NORMAL)
			{
				m_vTargetPos = pTargetItem->m_vPos;
			}
			else if(m_pTarget && ATTACK_AVAILABLE_STATE(m_pTarget->m_dwState))
			{
				m_vTargetPos = m_pTarget->m_vPos;
			}
			else
			{
				m_bSetTarget = FALSE;
			}

//			DBGOUT("%d. Ĺ¸°Ů(%.0f, %.0f, %.0f) ", m_nWeaponIndex, m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z);
//			DBGOUT("ąĚ»çŔĎ(%.0f, %.0f, %.0f) ", m_vPos.x, m_vPos.y, m_vPos.z);
//			DBGOUT("°Ĺ¸® = %.0f\n", D3DXVec3Length(&(m_vPos - m_vTargetPos)));
			// MoveWeapon ´ë˝Ĺ ĆĐĹĎ Ŕűżë
			BOOL bResult;							// ¸ńÇĄÁˇżˇ µµ´ŢÇß´ÂÁö °á°ú..
			bResult=m_pCinema->Tick(m_vTargetPos);	// °á°ú°ˇ FALSE ¸é ¸ńÇĄ¸¦ Áöłµ´Ů.. Áď µµ´ŢÇß´Ů.
			EVENT_POINT ep;
			ep=m_pCinema->GetCurrentCinemaPoint();
			m_vPos = ep.vPosition;			// Ŕ§Äˇ
			m_vVel = ep.vDirection;			// ąćÇâ
			m_fWeaponSpeed = ep.fVelocity;	// ĽÓ·Â
			m_vUp = ep.vUpVector;
			vTargetPosition = ep.vTarget;

			// ł»°ˇ ąß»çÇŃ ą«±âŔĚ°ĹłŞ ¸ó˝şĹÍ°ˇ ąß»çÇŃ 2Çü ą«±âŔĎ¶§ Ăćµą ĂĽĹ©
			if(	m_nClientIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex ||
				(IS_SECONDARY_WEAPON_MONSTER(m_pItemData->Kind) &&
				m_nTargetIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex ))
			{
				// to avoid lag bombing issues (frozen client calculates with 5 FPS (0.2f) elapsed tick
				//float fMovingDistance = ep.fVelocity*fElapsedTime;
				float fMovingDistance = ep.fVelocity * min(fElapsedTime, max_bombing_mode_elapsed);
				CheckTargetByBomb(fMovingDistance);
			//	float fMovingDistance = ep.fVelocity*fElapsedTime;
			//	CheckTargetByBomb(fMovingDistance);
			}
		}
	}
	else if(m_dwWeaponState == _EXPLODED)
	{
		if(m_pCharacterInfo)
		{
			if(!m_pCharacterInfo->IsUsing())
				m_bUsing = FALSE;
		}
		else
		{
			m_bUsing = FALSE;
		}
	}
	D3DXVECTOR3 vSide;
	D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
	D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);

	// ĆĐĹĎ Ĺ¸ÄĎ ąćÇâ °ü·Ă 2004-07-27 jschoi
	if(nPatternType == TARGET_OFF)
	{
		D3DXMatrixLookAtRH( &m_mMatrix, &(m_vPos), &(m_vPos + m_vVel), &m_vUp);
	}
	else
	{
		D3DXMatrixLookAtRH( &m_mMatrix, &(m_vPos), &(vTargetPosition), &m_vUp);
	}

	D3DXMatrixInverse( &m_mMatrix, NULL, &m_mMatrix );
	// effect matrix & ticking
	if(m_pCharacterInfo)
	{
		m_pCharacterInfo->SetAllBodyConditionMatrix(m_mMatrix );
//		m_pCharacterInfo->SetSingleBodyConditionMatrix( BODYCON_FIRE_MASK,m_mFireMatrix );
		m_pCharacterInfo->Tick(fElapsedTime);
	}
}

void CWeaponMissileData::CheckTargetByBomb(float fMovingDistance)
{
	int nTargetIndex;
	CItemData* pTargetItem = NULL;
	BOOL bCollision = FALSE;
	float fExplosionRange = 0;

	// ABM/GBM on monsters
	if (IS_SECONDARY_WEAPON_MONSTER(m_pItemData->Kind) && IS_MONSTER_CLIENT_INDEX(m_nClientIndex))
	{
		float fLengthMin = 1000.0f;
		CMonsterData* mon_data = reinterpret_cast<CMonsterData*>(m_pAttacker);
		if (!IS_SAME_CHARACTER_MONSTER_INFLUENCE(g_pShuttleChild->m_myShuttleInfo.InfluenceType, mon_data->m_pMonsterInfo->Belligerence))
		{
			if (SKILL_BASE_NUM(m_nSkillNum) != BGEAR_SKILL_BASENUM_GROUNDBOMBINGMODE
				//&& SKILL_BASE_NUM(m_nSkillNum) != BGEAR_SKILL_BASENUM_CUSTOMGROUNDBOMBINGMODE
				)
			{
				fExplosionRange = m_fExplosionRange;
			}
			else
			{
				//  Using 1/10 of the gbm available proxy
				fExplosionRange = m_fExplosionRange / 10;
			}

			float fLengthTemp = D3DXVec3Length(&(m_vPos - g_pShuttleChild->m_vPos));

			if (ATTACK_AVAILABLE_STATE(g_pShuttleChild->m_dwState)
				// Removed fMovingDistance parameter in order to prevent lagbombings
				&& fLengthTemp < g_pShuttleChild->m_fObjectSize /*+ fMovingDistance*/ + fExplosionRange)
			{
				if (fLengthMin > fLengthTemp)
				{


					if (!g_pShuttleChild->m_bRollUsed || g_pShuttleChild->m_fRollTime > ROLLING_USE_TIME)
					{
						nTargetIndex = g_pShuttleChild->GetShuttleInfo()->ClientIndex;
						pTargetItem = g_pScene->FindFieldItemByPartTypeAndParent(_DUMMY, g_pShuttleChild);
						fLengthMin = fLengthTemp;
						bCollision = TRUE;
					}
				}
			}
		}

	return;

	}

	// Enemy �浹 �˻�

	if (!bCollision)
	{

		float fLengthMin = 1000.0f;
		CEnemyData* pCollisionTarget = NULL;
		CMapEnemyIterator it = g_pScene->m_mapEnemyList.begin();
		while( it != g_pScene->m_mapEnemyList.end() )
		{
			if(	it->second->IsPkEnable())
			{
				CEnemyData* pTarget = it->second;
				float fLengthTemp = D3DXVec3Length(&(m_vPos - pTarget->m_vPos));
				fExplosionRange = 0;
				if (SKILL_BASE_NUM(m_nSkillNum) != BGEAR_SKILL_BASENUM_GROUNDBOMBINGMODE
					//&& SKILL_BASE_NUM(m_nSkillNum) != BGEAR_SKILL_BASENUM_CUSTOMGROUNDBOMBINGMODE
					)
				{
					fExplosionRange = m_fExplosionRange;
				}
				else
				{
					// Using 1/10 of the gbm available proxy
					fExplosionRange = m_fExplosionRange / 10;
				}


				// 2007-04-30 by bhsohn Ÿ�� ����
				if( ATTACK_AVAILABLE_STATE(it->second->m_dwState)
					// Removed fMovingDistance parameter in order to prevent lagbombings
				&& pTarget && fLengthTemp < pTarget->m_fObjectSize /*+ fMovingDistance*/ + fExplosionRange)
				{
					if(fLengthMin > fLengthTemp)
					{

						// Bombing debug parameters
#ifdef F_TEST
						char buf[128];
						sprintf(buf, "T:[%s] MD:[%f] ER:[%f] L:[%f]", pTarget->m_Info.CharacterInfo.CharacterName, fMovingDistance, fExplosionRange, fLengthTemp);
						g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_SYSTEM);
#endif
						pCollisionTarget = pTarget;
						nTargetIndex = pTarget->m_infoCharacter.CharacterInfo.ClientIndex;
						pTargetItem = g_pScene->FindFieldItemByPartTypeAndParent( _DUMMY, pTarget );
						fLengthMin = fLengthTemp;
						bCollision = TRUE;
					}
				}
			}
			it++;
		}

		BOOL bCheckRolling = RollingCollision(pCollisionTarget);
		if(bCheckRolling == FALSE)
		{
			bCollision = FALSE;
		}
	}

	// 2013-02-18 by bhsohn ����/���� ������ �ȵ���� ���� ó��
	char chDebugColl[256] = "";
	// END 2013-02-18 by bhsohn ����/���� ������ �ȵ���� ���� ó��

	// Monster �浹 �˻�
	if(!bCollision && !IS_SECONDARY_WEAPON_MONSTER(m_pItemData->Kind) && !IS_MONSTER_CLIENT_INDEX(m_nClientIndex))
	{
		float fLengthMin = 1000.0f;
		CMapMonsterIterator it = g_pScene->m_mapMonsterList.begin();
		while( it != g_pScene->m_mapMonsterList.end() )
		{
			CMonsterData* pTarget = it->second;

			float fLengthTemp = D3DXVec3Length(&(m_vPos - pTarget->m_vPos));
			fExplosionRange = 0;
			// 2013-02-18 by bhsohn ����/���� ������ �ȵ���� ���� ó��
//			if( SKILL_BASE_NUM(m_nSkillNum) != BGEAR_SKILL_BASENUM_GROUNDBOMBINGMODE)
			if( SKILL_BASE_NUM(m_nSkillNum) == BGEAR_SKILL_BASENUM_GROUNDBOMBINGMODE
			//	|| SKILL_BASE_NUM(m_nSkillNum) == BGEAR_SKILL_BASENUM_CUSTOMGROUNDBOMBINGMODE
				)
			{
				fExplosionRange = m_fExplosionRange;
			}
			// 2013-03-21 by bhsohn ���� ������ ���� �ȵ���� ���� �ذ�
			else if( SKILL_BASE_NUM(m_nSkillNum) == BGEAR_SKILL_BASENUM_AIRBOMBINGMODE)
			{
				fExplosionRange = m_fExplosionRange;
			}
			// END 2013-03-21 by bhsohn ���� ������ ���� �ȵ���� ���� �ذ�

			if( pTarget && fLengthTemp <
				(pTarget->m_pMonMesh!=NULL?pTarget->m_pMonMesh->m_fRadius : pTarget->m_fObjectSize) + fExplosionRange)
			{
				// 2007-05-16 by bhsohn ���� ���� ������ ����/���� ������, ������ ���� ó��
				//if(fLengthMin > fLengthTemp)
				// ���� ������ �ƴϾ��߸� �Ѵ�.
				if(!IS_SAME_CHARACTER_MONSTER_INFLUENCE(g_pShuttleChild->m_myShuttleInfo.InfluenceType, pTarget->m_pMonsterInfo->Belligerence)
					&& (fLengthMin > fLengthTemp))
				{
					float fMonsterRadius;
					if (pTarget->m_pMonMesh)
					{
						fMonsterRadius = pTarget->m_pMonMesh->m_fRadius;
					}
					else
					{
						fMonsterRadius = pTarget->m_fObjectSize;
					}


					if(pTarget->m_dwPartType == _MONSTER && fMonsterRadius > BIG_MONSTER_SIZE)
					{
						//*--------------------------------------------------------------------------*//
						// 2006-12-13 by ispark, ������ �޽� �浹 �˻�
						D3DXVECTOR3 vSide,vUpTemp;
						D3DXMATRIX matMonster;
						D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
						D3DXVec3Cross(&vUpTemp,&m_vVel,&vSide);
						D3DXMatrixLookAtLH( &matMonster, &m_vPos, &(m_vPos+m_vVel), &vUpTemp);
						pTarget->m_pMonMesh->Tick(pTarget->m_fCurrentTime);
						pTarget->m_pMonMesh->SetWorldMatrix(pTarget->m_mMatrix);
						float fcollDist = pTarget->m_pMonMesh->CheckCollision(matMonster,m_vPos,no_hit_distance,TRUE,FALSE).fDist;
						// 2013-02-18 by bhsohn ����/���� ������ �ȵ���� ���� ó��
//						if(fcollDist <= CAtumSJ::GetExplosionRange(m_pItemData, &g_pShuttleChild->m_paramFactor))
//						DBGOUT("fcollDist[%.2f] fExplosionRange[%.2f] #2 \n", fcollDist, fExplosionRange);
						sprintf(chDebugColl, "fcollDist[%.2f] fExplosionRange[%.2f] #1 \n", fcollDist, fExplosionRange);
						if(fcollDist <= fExplosionRange)
						{
							nTargetIndex = pTarget->m_info.MonsterIndex;
							pTargetItem = g_pScene->FindFieldItemByPartTypeAndParent( _DUMMY, pTarget );
							fLengthMin = fLengthTemp;
							bCollision = TRUE;
						}
						//*--------------------------------------------------------------------------*//							nTargetIndex = pTarget->m_info.MonsterIndex;
					}
					else
					{
						nTargetIndex = pTarget->m_info.MonsterIndex;
						pTargetItem = g_pScene->FindFieldItemByPartTypeAndParent( _DUMMY, pTarget );
						fLengthMin = fLengthTemp;
						bCollision = TRUE;
					}
				}
			}
			it++;
		}
	}

	//���� �浹
	if(!bCollision)
	{
		D3DXVECTOR3 vSide,vUpTemp;
		D3DXMATRIX mat;
		D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
		D3DXVec3Cross(&vUpTemp,&m_vVel,&vSide);
		D3DXMatrixLookAtLH( &mat, &m_vPos, &(m_vPos+m_vVel), &vUpTemp);
		float fDist = g_pScene->m_pObjectRender->CheckCollMeshRangeObject(mat,m_vPos,fMovingDistance).fDist;
		if(fDist < fMovingDistance)
		{
			nTargetIndex = 0;
			bCollision = TRUE;
		}

		if(!bCollision)
		{
			// �ٴ� �浹
			float fHeight = g_pGround->CheckHeightMap(m_vPos);
			if(fHeight > m_vPos.y)
			{
				nTargetIndex = 0;
				bCollision = TRUE;
			}
		}

		// 2006-12-01 by ispark, ���� ����
		if(!bCollision &&
			SKILL_BASE_NUM(m_nSkillNum) == BGEAR_SKILL_BASENUM_AIRBOMBINGMODE
			&& !IS_SECONDARY_WEAPON_MONSTER(m_pItemData->Kind) && !IS_MONSTER_CLIENT_INDEX(m_nClientIndex))
		{
			float fLength = D3DXVec3Length(&(m_vPos - m_vTargetPos));
			if(fLength < 50.0f)
			{
				nTargetIndex = 0;
				bCollision = TRUE;
			}
		}
		// 2007-10-04 by dgwoo ���� ����.
		fExplosionRange = 0;
		if( SKILL_BASE_NUM(m_nSkillNum) == BGEAR_SKILL_BASENUM_GROUNDBOMBINGMODE
			//|| SKILL_BASE_NUM(m_nSkillNum) == BGEAR_SKILL_BASENUM_CUSTOMGROUNDBOMBINGMODE
			)
		{
			fExplosionRange = m_fExplosionRange;
		}



		// 2004-10-23 by jschoi
		//////////////////////////////////////////////////////////////////////////
		// �ٴ� �浹
		// ���⼭ ������, Enemy �� ���߹ݰ濡 ���� �浹 ó���ؾ���.
#ifdef _INET_DES_LOCK8COLL
		if(	bCollision && nTargetIndex == 0 && !bDesForbidColl)
#else
		if (bCollision && nTargetIndex == 0)
#endif
		{
			float fLengthMin = 1000.0f;
			CEnemyData* pCollisionTarget = NULL;
			CMapEnemyIterator it = g_pScene->m_mapEnemyList.begin();
			while( it != g_pScene->m_mapEnemyList.end() )
			{
				if(	it->second->IsPkEnable())
				{
					CEnemyData* pTarget = it->second;
					float fLengthTemp = D3DXVec3Length(&(m_vPos - pTarget->m_vPos));

					// 2007-04-30 by bhsohn Ÿ�� ����
					if( ATTACK_AVAILABLE_STATE(it->second->m_dwState)
						&& pTarget && fLengthTemp < pTarget->m_fObjectSize + fMovingDistance + m_fExplosionRange)
					{
						if(fLengthMin > fLengthTemp)
						{
							pCollisionTarget = pTarget;
							nTargetIndex = pTarget->m_infoCharacter.CharacterInfo.ClientIndex;
							pTargetItem = g_pScene->FindFieldItemByPartTypeAndParent( _DUMMY, pTarget );
							fLengthMin = fLengthTemp;
							bCollision = TRUE;
						}
					}
				}
				it++;
			}

			BOOL bCheckRolling = RollingCollision(pCollisionTarget);
			if(bCheckRolling == FALSE)
			{
				bCollision = FALSE;
			}
		}

		if(	bCollision && nTargetIndex == 0 && !IS_SECONDARY_WEAPON_MONSTER(m_pItemData->Kind) && !IS_MONSTER_CLIENT_INDEX(m_nClientIndex))
		{
			float fLengthMin = 1000.0f;
			CMapMonsterIterator it = g_pScene->m_mapMonsterList.begin();
			while( it != g_pScene->m_mapMonsterList.end() )
			{
				CMonsterData* pTarget = it->second;
				float fLengthTemp = D3DXVec3Length(&(m_vPos - pTarget->m_vPos));
// 2007-10-04 by dgwoo ������ ź�� �����Ǿ��ִ� ���߹ݰ��� �����Ѵ�.
//				if( pTarget && fLengthTemp <
//					(pTarget->m_pMonMesh!=NULL?pTarget->m_pMonMesh->m_fRadius : pTarget->m_fObjectSize) + CAtumSJ::GetExplosionRange(m_pItemData, &g_pShuttleChild->m_paramFactor))
				if( pTarget && fLengthTemp <
					(pTarget->m_pMonMesh!=NULL?pTarget->m_pMonMesh->m_fRadius : pTarget->m_fObjectSize) + fExplosionRange)
				{
					if(fLengthMin > fLengthTemp)
					{
						float fMonsterRadius = pTarget->m_pMonMesh->m_fRadius;
						if(pTarget->m_dwPartType == _MONSTER && fMonsterRadius > BIG_MONSTER_SIZE)
						{
							//*--------------------------------------------------------------------------*//
							// 2006-12-13 by ispark, ������ �޽� �浹 �˻�
							D3DXVECTOR3 vSide,vUpTemp;
							D3DXMATRIX matMonster;
							D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
							D3DXVec3Cross(&vUpTemp,&m_vVel,&vSide);
							D3DXMatrixLookAtLH( &matMonster, &m_vPos, &(m_vPos+m_vVel), &vUpTemp);
							pTarget->m_pMonMesh->Tick(pTarget->m_fCurrentTime);
							pTarget->m_pMonMesh->SetWorldMatrix(pTarget->m_mMatrix);
							float fcollDist = pTarget->m_pMonMesh->CheckCollision(matMonster,m_vPos,no_hit_distance,TRUE,FALSE).fDist;
							// 2013-02-18 by bhsohn ����/���� ������ �ȵ���� ���� ó��
//							if(fcollDist <= CAtumSJ::GetExplosionRange(m_pItemData, &g_pShuttleChild->m_paramFactor) )
//							DBGOUT("fcollDist[%.2f] fExplosionRange[%.2f] #1 \n", fcollDist, fExplosionRange);
							sprintf(chDebugColl, "fcollDist[%.2f] fExplosionRange[%.2f] #2 \n", fcollDist, fExplosionRange);

							if(fcollDist <= fExplosionRange)
							{
								nTargetIndex = pTarget->m_info.MonsterIndex;
								pTargetItem = g_pScene->FindFieldItemByPartTypeAndParent( _DUMMY, pTarget );
								fLengthMin = fLengthTemp;
								bCollision = TRUE;
							}
							//*--------------------------------------------------------------------------*//							nTargetIndex = pTarget->m_info.MonsterIndex;
						}
						else
						{
							nTargetIndex = pTarget->m_info.MonsterIndex;
							pTargetItem = g_pScene->FindFieldItemByPartTypeAndParent( _DUMMY, pTarget );
							fLengthMin = fLengthTemp;
							bCollision = TRUE;
						}
					}
				}
				it++;
			}
		}
	}

	if(bCollision)
	{
		SendFieldSocketBattleAttackFind(nTargetIndex, pTargetItem==NULL ? 0:pTargetItem->m_nItemIndex,m_nClientIndex,m_pItemData->ItemNum);

		// 2013-02-18 by bhsohn ����/���� ������ �ȵ���� ���� ó��
// 		int nMyClientIndex = m_nClientIndex;
// 		if(nMyClientIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex)
// 		{
// 			DBGOUT("CWSlowData::SendFieldSocketBattleAttackFind nClientIndex[%d] nTargetIndex[%d] [%s] \n",
// 				nMyClientIndex, nTargetIndex, chDebugColl);
// 		}
		// END 2013-02-18 by bhsohn ����/���� ������ �ȵ���� ���� ó��

		m_dwWeaponState = _EXPLODING;
	}

}

void CWeaponMissileData::CheckTargetWarning()
{
	if(g_pShuttleChild && (m_nTargetMe == g_pShuttleChild->m_myShuttleInfo.ClientIndex))
	{
		int nMissileCount = g_pShuttleChild->GetMissileCount();
		nMissileCount--;
//		DBGOUT("»čÁ¦ %d\n", nMissileCount);
		g_pShuttleChild->SetMissileCount(nMissileCount);
		if(nMissileCount <= 0)
		{
			g_pShuttleChild->SetMissileWarning(FALSE);
			g_pShuttleChild->SetMissileCount(0);
		}
		m_nTargetMe = -1;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CWeaponMissileData::CheckTargetState()
/// \brief		Ĺ¸°Ů »óĹÂ °Ë»ç
/// \author		ispark
/// \date		2006-12-08 ~ 2006-12-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CWeaponMissileData::CheckTargetState()
{
	//////////////////////////////////////////////////////////////////////////
	// ł»°ˇ Ĺ¸°ŮŔĚ¸é
	if(m_nTargetIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex)
	{
		if((m_pAttacker->m_dwPartType == _MONSTER &&				// ŔÎşńÁöşíŔ» ŔÎ˝ÄÇĎ´Â ¸ó˝şĹÍŔĚ°ĹłŞ .
			COMPARE_MPOPTION_BIT(((CMonsterData*)m_pAttacker)->m_pMonsterInfo->MPOption,MPOPTION_BIT_RECOGNIZE_INVISIBLE))
			|| g_pShuttleChild->m_bySkillStateFlag == CL_SKILL_NONE) //˝şĹłŔ» ąßµżÁßŔĚ ľĆ´Ň¶§.
		{// 
		}
		else
		{
			// 2007-04-24 by bhsohn ł»°ˇ ŔÎşńÁöşí »óĹÂŔĎ‹š ąĚ»çŔĎŔĚ ŔĚ»óÇŃ °÷żˇ ĹÍÁö´Â Çö»óĂł¸®
			//m_pTarget = NULL;
			if(m_pAttacker->m_dwPartType != _ENEMY)
			{
				m_pTarget = NULL;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Ĺ¸°ŮŔĚ »ó´ëąćŔĚ¸é
	if( m_pTarget && m_pTarget->m_dwPartType == _ENEMY)
	{
		// 2006-12-08 by ispark, ŔÎşńÁöşí, Ŕ§ŔĺŔĚ¶ó¸é Ĺ¸°ŮŔ» ŔŇ´Â´Ů.
		//if(((CEnemyData *)m_pTarget)->m_bySkillStateFlag != CL_SKILL_NONE)
		// 2007-02-08 by dgwoo ľËĆÄ °Şżˇ µű¶ó Ĺ¸°ŮŔ» ŔŇŔ»¶§.
		if(((CEnemyData *)m_pTarget)->m_nAlphaValue == SKILL_OBJECT_ALPHA_OTHER_INFLUENCE)
		{
			// 2007-06-15 by dgwoo ŔĚąĚ Ĺ¸°ŮŔ» ÇŃąř ŔâŔş ąĚ»çŔĎŔş °čĽÓ µű¶ó°ˇµµ·Ď ĽöÁ¤.
			//m_pTarget = NULL;
		}
	}
}