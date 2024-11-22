This method is for incredibly fast serialization of object graphs in c++

Ideally you would sort your game objects (such as vertex and index arrays for 3d meshes that go into the millions of counts) by a database index, and that produces a linked list.

This method avoids the overheads of copying and object initialization AND individual field read/writing, meaning 5m vertices = 15m object creation calls are avoided. If your terrain has normals for lighting (usually does!) then that is 30 million object constructions that are completely avoided.

This method massively increases video game level loading speeds for PS5 and Xbox Series X and future consoles, for example. Future-proof method of class organization for MMOs.
