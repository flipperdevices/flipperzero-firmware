typedef struct
{
    int currentPage;
    int start;
    int end;
    int position;
} PageInfo;

typedef struct
{
    int start;
    int end;
    int position;
} ContinuousPageInfo;

PageInfo pagination_paged(int totalElements, int elementsPerPage, int selectedElement);
ContinuousPageInfo pagination_continuous_centered(int totalElements, int elementsPerPage, int selectedElement);