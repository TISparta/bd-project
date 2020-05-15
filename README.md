# DBII Project

# Requisites
- C++14-standard compiler
- Make
- GTest

# Run tests
```bash
cd test
make
./bplustree-test
```

# Generate graphics
```bash
cd performance
make
./measures
python3 -m venv venv
source venv/bin/activate
python generate-graphics.py
```

# Proof of concept of join
```bash
cd join
make
./join
```

