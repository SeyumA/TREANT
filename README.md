# TREANT

This repository contains the source code associated with the method proposed by Calzavara _et al._ in their ... research paper entitled "_TREANT: Training Evasion-Aware Decision Trees_" \[more information available at: [arXiv.org](https://arxiv.org/abs/1907.01197)\]<br />
The name [TREANT](https://en.wikipedia.org/wiki/Treant) comes from the "_Dungeons & Dragons_" roleplaying game, where it identifies giant tree-like fictional creatures.


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
