#include <windows.h>
#include <tchar.h>

#include "dicomtagconfig.h"
#include "dicomdictionary.h"
#include "dicomdecoder.h"
#include "dicomtagstore.h"
#include "stools.h"

using namespace DicomImageViewer;

////////////////////////////////////////////////////////////////////////////////

bool tagSortProc( const TagElement* first, const TagElement* second );

////////////////////////////////////////////////////////////////////////////////

TagStore::TagStore()
:bLittleEndian(DATA_ARRANGE_LITTLE_ENDIAN)
{
    // configure me --
}

TagStore::~TagStore()
{
    clearTags();
}

void TagStore::clearTags()
{
    int nTagCount = TagElements.size();
    if( nTagCount == 0)
        return;

    // do iterator ...
    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        TagElement* pTE = *it;

        if(pTE)
        {
            if(pTE->size && pTE->dynamicbuffer && pTE->alloced)
            {
                delete[] pTE->dynamicbuffer;
                pTE->alloced = FALSE;
            }
        }
    }

    while(!TagElements.empty())
    {
        delete TagElements.back();
        TagElements.pop_back();
    }
}

bool TagStore::IsCreated()
{
    return true;
}

DWORD TagStore::GetTagCount()
{
    return TagElements.size();
}

TagElement* TagStore::FindTagElement(DWORD TagID)
{
    if (TagID == 0)
        return NULL;

    DWORD nCnt = 0;

    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        TagElement *pE = *it;
        if(pE)
        {
            if(pE->id == TagID)
                return pE;
        }
    }

    return NULL;
}

DWORD TagStore::AddTagElement(DWORD TagID,WORD wVR, char* data, DWORD size)
{
    TagElement *pNewTag = new TagElement;

    // erase it as NULL, first.
    memset(pNewTag,0,sizeof(TagElement));

    pNewTag->id = TagID;
    memcpy(pNewTag->VRtype,&wVR,2);
    pNewTag->size = size;
    if(size)
    {
        if(size > MAX_STATICBUFFER_LENGTH)
        {
            pNewTag->dynamicbuffer = new char[size];
            pNewTag->alloced = true;
            memcpy(pNewTag->dynamicbuffer,data,size);
        }else{
            memset(pNewTag->staticbuffer,0,MAX_STATICBUFFER_LENGTH);
            memcpy(pNewTag->staticbuffer,data,size);
			pNewTag->alloced = false;
        }
    }

    TagElements.push_back(pNewTag);

    return TagElements.size();
}

int  TagStore::AddTagElement(TagElement *pTag)
{
    if(!pTag)
        return -1;

    // check same elements by ID.
    if(FindTagElement(pTag->id))
        return -2;

    TagElements.push_back(pTag);

    return TagElements.size();
}

TagElement* TagStore::GetTagElement(DWORD nIndex)
{
    if (nIndex > TagElements.size())
        return NULL;

    DWORD nCnt = 0;

    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        if(nCnt == nIndex)
        {
            return *it;
        }
        nCnt++;
    }

    return NULL;
}

void TagStore::Sort()
{
    TagElements.sort( tagSortProc );
}

////////////////////////////////////////////////////////////////////////////////

bool tagSortProc( const TagElement* first, const TagElement* second )
{
    if ( second->id > first->id )
        return true;

    return false;
}
