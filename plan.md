## Initial plan
1- Add a container that has the clang p2996 branch install there. Similar to what is done in the ../rbb repo on my machine and what is done in the simdjson repo as well.
2- Add reflection based logic to create the bindings for the classes of interest, don't be shy to use C++ concepts to make the code really generic and to support a broader set of classes. For now we are starting with python, so focus on Python when creating the new bindings.
3- I want this project to follow the single-header philosophy, I want to keep things simple and bloat inexistent.
4- As an acceptance criteria, I want use to have a test folder that has the end-to-end test of:
    4.1 - Adding a new C++ class
    4.2 - Making that available to python via the adding the class to the central registry and calling the script to update the images.
    4.3 - Running python code that uses the binding and making sure that the output matches the expectation.
5- Document the solution in write_up.md
6- Whenever you've achieved a critical milestone, stop the work and ask me to review it so that we can push it to the repo. Never add yourself as an author.




