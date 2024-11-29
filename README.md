Warning: I use a struct instead of a class for the object graph serialization because of the vtable versioning issues with loading an old data file in your project. As long as the struct fields stay the same, the version of the file is effectively the same (this way) no matter the modification of the class that's being deserialized.

This method is for incredibly fast serialization of object graphs in c++

Ideally you would sort your game objects (such as vertex and index arrays for 3d meshes that go into the millions of counts) by a database index, and that produces a linked list.

This method avoids the overheads of copying and object initialization AND individual field read/writing, meaning 5m vertices = 15m object creation calls are avoided. If your terrain has normals for lighting (usually does!) then that is 45 million+ object constructions that are completely avoided. (15m float for vertex positions, 15m float for vertex normals, 15m vector3 constructor calls, and then what about texture information).

This method massively increases video game level loading speeds for PS5 and Xbox Series X and future consoles, for example. Future-proof method of class organization for MMOs.
