#include "pagination.h"
#include "calc.h"

PageInfo pagination_paged(int totalElements, int elementsPerPage, int selectedElement)
{
    PageInfo pageInfo;
    pageInfo.currentPage = (selectedElement / elementsPerPage) + 1;

    pageInfo.start = (pageInfo.currentPage - 1) * elementsPerPage;
    pageInfo.end = clamp(pageInfo.start + elementsPerPage - 1, 0, totalElements - 1);

    pageInfo.position = selectedElement - pageInfo.start;

    return pageInfo;
}

ContinuousPageInfo pagination_continuous_centered(int totalElements, int elementsPerPage, int selectedElement)
{
    ContinuousPageInfo pageInfo;

    int halfToShow = elementsPerPage / 2;
    pageInfo.start = selectedElement - halfToShow;
    pageInfo.end = selectedElement + halfToShow;

    if (pageInfo.start < 0)
    {
        pageInfo.start = 0;
        pageInfo.end = clamp(elementsPerPage - 1, 0, totalElements - 1);
    }
    else if (pageInfo.end >= totalElements)
    {
        pageInfo.end = totalElements - 1;
        pageInfo.start = clamp(totalElements - elementsPerPage, 0, totalElements - 1);
    }

    pageInfo.position = selectedElement - pageInfo.start;

    return pageInfo;
}