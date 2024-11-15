/*
Copyright 2024 Gavin Taylor
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the �Software�), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <cstdint>

struct MemoryPool
{
public:
    void* start;
    void* memoryPtr;
    int objSizeInBytes;

    void* next() {
        memoryPtr = (void*)((intptr_t)objSizeInBytes + (intptr_t)memoryPtr);
        return memoryPtr;
    }
    void* new_alloc(int objSizeInBytes, int count)
    {
        if (memoryPtr)
            throw std::exception();

        this->objSizeInBytes = objSizeInBytes;
        memoryPtr = malloc(objSizeInBytes * count);
        start = memoryPtr;
        return memoryPtr;
    }
};





struct TerrainMesh
{
public:
    float vertices[256 * 256 * 4];
    short indices[(256 - 1) * (256 - 1) * 6];
    TerrainMesh* nextAbs;
    TerrainMesh* nextRel;

    void Fixup(MemoryPool& pool)
    {
        this->nextAbs = (TerrainMesh*)((intptr_t)pool.start + (intptr_t)this->nextRel);
    }

    TerrainMesh(MemoryPool& pool, TerrainMesh* next, int index)
    {
        this->nextAbs = next;
        this->nextRel = (TerrainMesh*)((intptr_t)sizeof(TerrainMesh)*index);

        //dummy values - proof of concept
        vertices[256 * 256 * 4 - 1] = 1;
        indices[((256 - 1) * (256 - 1) * 6) - 1] = 1;
    }
};


int main() {

    auto memoryPool = MemoryPool();

    auto terrainMesh4 = new (memoryPool.new_alloc(sizeof(TerrainMesh), 4)) TerrainMesh(memoryPool, 0, 3);
    auto terrainMesh3 = new (memoryPool.next()) TerrainMesh(memoryPool, terrainMesh4, 2);
    auto terrainMesh2 = new (memoryPool.next()) TerrainMesh(memoryPool, terrainMesh3, 1);
    auto terrainMesh1 = new (memoryPool.next()) TerrainMesh(memoryPool, terrainMesh2, 0);

    //serialize to file
    auto ptr = (unsigned char*)((intptr_t)terrainMesh1);
    auto length = memoryPool.objSizeInBytes * 4;

    //read in and check our new data
    auto memoryPool2 = MemoryPool();
    memoryPool2.objSizeInBytes = sizeof(TerrainMesh);
    //unsigned char* inByteArray = new unsigned char[memoryPool2.objSizeInBytes * 4];
    //from here you would use fread() to deserialize from a file. size=1, length=memoryPool2.objSizeInBytes * 4
    memoryPool2.memoryPtr = ptr;//inByteArray;
    memoryPool2.start = ptr;//inByteArray;
    auto terrainMesh_2_1 = reinterpret_cast<TerrainMesh*>(memoryPool2.start);
    bool success1 = false;
    bool success2 = false;
    terrainMesh_2_1->Fixup(memoryPool2);
    terrainMesh_2_1->nextAbs->Fixup(memoryPool2);
    terrainMesh_2_1->nextAbs->nextAbs->Fixup(memoryPool2);
    terrainMesh_2_1->nextAbs->nextAbs->nextAbs->Fixup(memoryPool2);
    if (terrainMesh_2_1->nextAbs->nextAbs->nextAbs->vertices[256 * 256 * 4 - 1] == 1)
        success1 = true;
    if (terrainMesh_2_1->nextAbs->nextAbs->nextAbs->indices[((256 - 1) * (256 - 1) * 6) - 1] == 1)
        success2 = true;
    if (!success1 || !success2) {
        std::cout << "Did not work." << std::endl;
        throw new std::exception(); //failed to recreate the data exactly.
    }

    std::cout << "Worked!" << std::endl;
    return 0;
}
