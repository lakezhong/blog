#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <set>

#define COMPANY_COUNT_MAX 200000
#define INTERVAL_TIME_MAX 1000000000
#define INTERVAL_TIME_INFINITE (INTERVAL_TIME_MAX + 1)

struct INTERVAL_S
{
    int iLeft;
    int iRight;

    bool operator < (const INTERVAL_S& stOther) const
    {
        return iRight == stOther.iRight ? iLeft > stOther.iLeft : iRight < stOther.iRight;
    }
};

INTERVAL_S g_astIntervals[COMPANY_COUNT_MAX];
INTERVAL_S g_astOperationIntervals[COMPANY_COUNT_MAX];
int g_aiEffectIntervalLefts[COMPANY_COUNT_MAX];
int g_aiEffectIntervalRights[COMPANY_COUNT_MAX];
int g_iEffectCompanyCount;
int g_aaiSparseTable[COMPANY_COUNT_MAX][19]; // 2^18 > COMPANY_COUNT_MAX

void PrepareSparseTable(INTERVAL_S* pstInput, int iInputSize)
{
    int i = 0;
    int j = 0;
    int iEffectIndex = 0;

    memcpy((void*)g_astOperationIntervals, (void*)g_astIntervals, sizeof(g_astIntervals));
    std::sort(g_astOperationIntervals, g_astOperationIntervals + iInputSize);

    // 去真包含
    for (iEffectIndex = 0, i = 1; i < iInputSize; ++i)
    {
        if ((g_astOperationIntervals[i].iLeft > g_astOperationIntervals[iEffectIndex].iLeft))
        {
            if (++iEffectIndex != i)
            {
                g_astOperationIntervals[iEffectIndex] = g_astOperationIntervals[i];
            }
        }
    }

    g_iEffectCompanyCount = iEffectIndex + 1;

    for (i = 0; i < g_iEffectCompanyCount; ++i)
    {
        printf("left = %d, right = %d\r\n", g_astOperationIntervals[i].iLeft, g_astOperationIntervals[i].iRight);
        g_aiEffectIntervalLefts[i] = g_astOperationIntervals[i].iLeft;
        g_aiEffectIntervalRights[i] = g_astOperationIntervals[i].iRight;
    }

    (void)memset((void*)g_aaiSparseTable, 0, sizeof(g_aaiSparseTable));

    // 计算每条线段右边第一条线段的索引
    for (i = 0, j = 0; i < g_iEffectCompanyCount; ++i)
    {
        while ((j < g_iEffectCompanyCount) && (g_astOperationIntervals[j].iLeft <= g_astOperationIntervals[i].iRight))
        {
            ++j;
        }

        g_aaiSparseTable[i][0] = (j >= g_iEffectCompanyCount ? 0 : j);
    }

    for (i = 0; i < g_iEffectCompanyCount; ++i)
    {
        printf("%d\r\n", g_aaiSparseTable[i][0]);
    }

    // 根据递推公式计算稀疏表
    for (j = 1; j < 19; ++j)
    {
        for (i = 0; i < g_iEffectCompanyCount; ++i)
        {
            if (g_aaiSparseTable[i][j - 1] != 0)
            {
                g_aaiSparseTable[i][j] = g_aaiSparseTable[g_aaiSparseTable[i][j - 1]][j - 1];
            }
        }
    }

    for (i = 0; i < g_iEffectCompanyCount; ++i)
    {
        for (j = 0; j < 19; ++j)
        {
            printf("%d ", g_aaiSparseTable[i][j]);
        }
        printf("\r\n");
    }

    return;
}

int GetRangeMax(int iLeft, int iRight)
{
    int iResult = 1;
    int j = 0;

    int iIndex = std::lower_bound(g_aiEffectIntervalLefts,
                                  g_aiEffectIntervalLefts + g_iEffectCompanyCount,
                                  iLeft)
                 - g_aiEffectIntervalLefts;

    if ((iIndex >= g_iEffectCompanyCount) || (g_aiEffectIntervalRights[iIndex] > iRight))
    {
        return 0;
    }

    for (j = 18; j >= 0; --j)
    {
        if ((g_aaiSparseTable[iIndex][j] != 0) && (g_aiEffectIntervalRights[g_aaiSparseTable[iIndex][j]] <= iRight))
        {
            iResult += (1 << j);
            iIndex = g_aaiSparseTable[iIndex][j];
        }
    }

    return iResult;
}

int Compute(INTERVAL_S* pstInput, int iInputSize, int* piOrders, int* piMaxCount)
{
    std::set<INTERVAL_S> intervalSet;
    std::set<INTERVAL_S>::iterator leftIt;
    std::set<INTERVAL_S>::iterator rightIt;
    INTERVAL_S stMin;
    INTERVAL_S stMax;
    int i = 0;
    int iChooseIndex = 0;

    if (iInputSize <= 0)
    {
        *piMaxCount = 0;
        return 0;
    }

    PrepareSparseTable(pstInput, iInputSize);

    stMin.iLeft     = -INTERVAL_TIME_INFINITE;
    stMin.iRight    = -INTERVAL_TIME_INFINITE;
    stMax.iLeft     =  INTERVAL_TIME_INFINITE;
    stMax.iRight    =  INTERVAL_TIME_INFINITE;

    intervalSet.insert(stMin);
    intervalSet.insert(stMax);

    for (i = 0; i < iInputSize; ++i)
    {
        rightIt = intervalSet.lower_bound(pstInput[i]);
        leftIt = rightIt;
        --leftIt;

        if ((pstInput[i].iLeft <= (*leftIt).iRight) || (pstInput[i].iRight >= (*rightIt).iLeft))
        {
            continue;
        }

        if (GetRangeMax((*leftIt).iRight + 1, (*rightIt).iLeft - 1)
            == GetRangeMax((*leftIt).iRight + 1, pstInput[i].iLeft - 1)
               + 1
               + GetRangeMax(pstInput[i].iRight + 1, (*rightIt).iLeft - 1))
        {
            intervalSet.insert(pstInput[i]);
            piOrders[iChooseIndex++] = i;
        }
    }

    *piMaxCount = iChooseIndex;

    return 0;
}

int main()
{
#if 0
    g_astIntervals[0].iLeft     = 1;
    g_astIntervals[0].iRight    = 10;

    g_astIntervals[1].iLeft     = 9;
    g_astIntervals[1].iRight    = 17;

    g_astIntervals[2].iLeft     = 1;
    g_astIntervals[2].iRight    = 17;

    g_astIntervals[3].iLeft     = 99;
    g_astIntervals[3].iRight    = 100;

    g_astIntervals[4].iLeft     = 19;
    g_astIntervals[4].iRight    = 20;

    g_astIntervals[5].iLeft     = 1;
    g_astIntervals[5].iRight    = 10;

    int aiOrders[6] = {0};

    int iMaxCount = 0;

    (void)Compute(g_astIntervals, 6, aiOrders, &iMaxCount);
#else
    g_astIntervals[0].iLeft     = 0;
    g_astIntervals[0].iRight    = 5;

    g_astIntervals[1].iLeft     = 2;
    g_astIntervals[1].iRight    = 9;

    g_astIntervals[2].iLeft     = 3;
    g_astIntervals[2].iRight    = 7;

    g_astIntervals[3].iLeft     = 8;
    g_astIntervals[3].iRight    = 1000000000; //17;

    g_astIntervals[4].iLeft     = 10;
    g_astIntervals[4].iRight    = 16;

    g_astIntervals[5].iLeft     = 6;
    g_astIntervals[5].iRight    = 16;

    g_astIntervals[6].iLeft     = 8;
    g_astIntervals[6].iRight    = 16;

    int aiOrders[7] = {0};

    int iMaxCount = 0;

    (void)Compute(g_astIntervals, 7, aiOrders, &iMaxCount);
#endif

    printf("iMaxCount = %d\n", iMaxCount);
    for (int i = 0; i < iMaxCount; ++i)
    {
        printf("%d ", aiOrders[i]);
    }

    printf("\n");
}
