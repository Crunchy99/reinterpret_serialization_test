/*
Copyright 2024 Gavin Taylor
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <cstdint>

template<typename T> struct MemoryPool
{
public:
    T* start;
    T* memoryPtr;

    //Manually construct the data within via new_alloc()
    MemoryPool(): start(0), memoryPtr(0) {}

    //Construct from file I/O byte buffer
    MemoryPool(unsigned char* bytesIn, int count)
    {
        memoryPtr = (T*)bytesIn;
        start = memoryPtr;

        auto next = start;
        for (int i = 0; i < count; i++)
        {
            next->Fixup(*this);
            next = next->nextAbs;
        }
    }

    T* next() {
        memoryPtr = (T*)((intptr_t)sizeof(T) + (intptr_t)memoryPtr);
        return memoryPtr;
    }
    T* new_alloc(int count)
    {
        if (memoryPtr)
            throw std::exception();

        memoryPtr = (T*)malloc(sizeof(T) * count);
        start = memoryPtr;
        return memoryPtr;
    }
};



class TerrainMesh
{
public:
    struct TerrainMeshStruct* Struct;

    void Initialize()
    {
        /* Your code here */
        /* Struct-> .. */
    }
    void Draw()
    {
        /* Your code here */
        /* Struct-> .. */
    }
};

struct TerrainMeshStruct
{
public:
    float vertices[256 * 256 * 4 * 3];
    short indices[(256 - 1) * (256 - 1) * 6];
    TerrainMeshStruct* nextRel;
    TerrainMeshStruct* nextAbs;
    TerrainMesh* Class;

    void Fixup(MemoryPool<TerrainMeshStruct>& pool)
    {
        this->nextAbs = (TerrainMeshStruct*)((intptr_t)pool.start + (intptr_t)this->nextRel);
        this->Class = new TerrainMesh();
        this->Class->Struct = this;
        //Here we would update any std::vector instances in program classes
    }

    //In this constructor you would want to add "TerrainMesh* Class" and set it
    TerrainMeshStruct(MemoryPool<TerrainMeshStruct>& pool, int index_next)
    {
        this->nextRel = (TerrainMeshStruct*)((intptr_t)sizeof(TerrainMeshStruct)*index_next);
        this->nextAbs = (TerrainMeshStruct*)((intptr_t)pool.start + (intptr_t)this->nextRel);

        //TODO: Initialize Class field here.

        //dummy values - proof of concept
        vertices[256 * 256 * 4 * 3 - 1] = 1;
        indices[((256 - 1) * (256 - 1) * 6) - 1] = 1;
    }
};


int main() {

    auto memoryPool = MemoryPool<TerrainMeshStruct>();

    auto terrainMesh1 = new (memoryPool.new_alloc(4)) TerrainMeshStruct(memoryPool, 1);
    auto terrainMesh2 = new (memoryPool.next()) TerrainMeshStruct(memoryPool, 2);
    auto terrainMesh3 = new (memoryPool.next()) TerrainMeshStruct(memoryPool, 3);
    auto terrainMesh4 = new (memoryPool.next()) TerrainMeshStruct(memoryPool, 0);

    //serialize to file
    auto ptr = (unsigned char*)((intptr_t)terrainMesh1);
    auto length = sizeof(TerrainMeshStruct) * 4;

    //read in and check our new data
    auto memoryPool2 = MemoryPool<TerrainMeshStruct>(ptr, 4);
    //unsigned char* inByteArray = new unsigned char[memoryPool2.objSizeInBytes * 4];
    //from here you would use fread() to deserialize from a file. size=1, length=sizeof(TerrainMeshStruct) * 4
    auto terrainMesh_2_1 = memoryPool2.start;
    bool success1 = false;
    bool success2 = false;
    if (terrainMesh_2_1->nextAbs->nextAbs->nextAbs->vertices[256 * 256 * 4 * 3 - 1] == 1)
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
