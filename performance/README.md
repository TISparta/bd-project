### Graphics of performance generation

### Configuration

* Create a virtual environment
- [Create a virtual environment](https://docs.python.org/3/tutorial/venv.html)

```bash
python3 -m venv venv
```

- Activate the virtual environment

* Windows
```
venv\Scripts\activate.bat
```

* Linux-MacOS
```
source venv/bin/activate
```

- Install dependencies
```
pip install -r requirements.txt
```

### Execution

```bash
make
./measures
python generate-graphics.py
```
