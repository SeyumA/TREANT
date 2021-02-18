# TreeAnt

This is a private repo to host the Msc Thesis work of Davide Girardini with Prof. Lucchese as supervisor.

Prof. Lucchese and Ph.D student Seyum will have permission to manage the repo.

In the project folder you must include a folder 3rdparty with the required third party libraries:
(see FindNlopt.cmake).

```bash
├── 3rdparty
│   └── nlopt-2.6.1
│       ├── include
│       └── lib
```

The nlopt is a library used to solve the optimization problem.
Follow the guide at:
https://nlopt.readthedocs.io/en/latest/NLopt_Installation/
install in local folder using:

```bash
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$HOME/install ..
```

(see "Changing the installation directory" chapter).
After:
```bash
make install
```
the installation folder will contain the include and lib folder.
Tested using static libraries: add -DBUILD_SHARED_LIBS=OFF to cmake flags.
