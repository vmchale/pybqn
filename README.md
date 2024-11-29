# BQN↔NumPy bridge

```python
>>> import numpy as np
>>> import bqn
>>> bqn.bqn('{(+´÷≠)˘7↕𝕩}',np.arange(0,10,dtype=np.int32))
array([3, 4, 5, 6], dtype=int8)
>>> bqn.bqn('¯1⊑(1⊸+⥊+)○≠(⌊`⊢⌊⊏⊸»∘⊢-0∾1+⊣)˝=⌜⟜⌽',"kitten","sitting")
3.0
```

## Installation

Install the BQN C library, `libcbqn.so` (`libcbqn.dylib` on Mac) to `/usr/local/lib`, and headers. Then:

```
pip3 install pybqn
```

## Limitations

- Only supports integer and floating-point types.
- Can't marshal Python integer scalars to BQN.
- Doesn't marshal BQN strings to Python
