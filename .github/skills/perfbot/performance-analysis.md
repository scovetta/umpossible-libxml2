# Performance Analysis

Methodology for identifying performance issues and benchmarking.

## Performance Assessment Process

### Phase 1: Identify Hot Paths

1. **Analyze the project architecture** — Which components handle the most data/traffic?
2. **Find computational hot spots** — Loops, recursive functions, data transformations
3. **Check I/O operations** — File reads, network calls, database queries
4. **Look for resource-intensive operations** — Sorting, searching, compression, encryption

### Phase 2: Anti-Pattern Detection

| Anti-Pattern | Impact | Detection |
|-------------|--------|-----------|
| O(n²) nested loops on large data | CPU bottleneck | Nested loops over input-sized collections |
| Allocation in hot loops | GC pressure / fragmentation | `malloc`/`new`/`append` inside performance-critical loops |
| Unbuffered I/O | Throughput reduction | Small reads/writes without buffering |
| Synchronous I/O in event loops | Blocking | `readFileSync`, blocking calls in async context |
| String concatenation in loops | O(n²) memory | `str += ...` in loops (Python, JS, Java) |
| Missing connection pooling | Resource exhaustion | New DB/HTTP connection per request |
| Excessive logging in hot paths | I/O bottleneck | Log calls in tight loops |
| Lock contention | Parallelism bottleneck | Shared mutex in frequently-called code |
| Cache-unfriendly access patterns | Memory latency | Strided or random access over large arrays |
| Redundant computation | Wasted CPU | Same calculation repeated without caching |

### Phase 3: Benchmark Evidence

**Every performance finding MUST include evidence.** No speculative performance claims.

Evidence types:
1. **Algorithmic analysis** — Big-O complexity with concrete sizes
2. **Profiling data** — If available from CI or benchmarks
3. **Benchmark results** — Before/after measurements
4. **Resource measurements** — Memory usage, allocation counts

## Ecosystem-Specific Benchmarks

### C/C++
```bash
# Compile with optimizations for benchmarks
gcc -O2 -o bench bench.c -lz
time ./bench < testdata

# Valgrind for memory analysis (Linux)
valgrind --tool=callgrind ./bench < testdata
valgrind --tool=massif ./bench < testdata
```

### Python
```python
import timeit
result = timeit.timeit('func()', setup='from module import func', number=1000)

# Memory profiling
from memory_profiler import profile
@profile
def func():
    ...
```

### JavaScript/Node.js
```javascript
console.time('operation');
// ... operation ...
console.timeEnd('operation');

// Or benchmark.js
const Benchmark = require('benchmark');
new Benchmark.Suite()
  .add('implementation', () => { /* ... */ })
  .run();
```

### Go
```go
func BenchmarkOperation(b *testing.B) {
    for i := 0; i < b.N; i++ {
        operation()
    }
}
// Run: go test -bench=. -benchmem
```

### Rust
```rust
#[bench]
fn bench_operation(b: &mut Bencher) {
    b.iter(|| operation());
}
// Or criterion.rs for stable benchmarks
```

## Performance Thresholds

| Metric | Good | Acceptable | Investigate |
|--------|------|------------|-------------|
| Hot loop complexity | O(n) or O(n log n) | O(n²) on small n | O(n²) on large n, O(n³)+ |
| Memory per operation | Constant | Linear with input | Quadratic+ with input |
| Allocation count in loop | 0 (pre-allocated) | Few | Proportional to iterations |
| I/O calls per operation | Batched/buffered | Few | One per item |

## Finding Format

```markdown
### Performance Finding: [Brief Title]

- **Location**: `file.c`, function `process_data()`, lines X-Y
- **Issue**: [Description of the anti-pattern]
- **Complexity**: O(n²) where n = [describe n]
- **Evidence**: [Algorithmic analysis or benchmark data]
- **Impact**: [Estimated impact — e.g., "50ms → 5ms for typical input"]
- **Recommendation**: [Specific optimization with expected improvement]
- **Risk**: [Any risks of the optimization — correctness, readability]
```

## Rules

1. **Never claim a performance issue without evidence**
2. **Prefer algorithmic improvements over micro-optimizations**
3. **Don't optimize code that isn't in a hot path**
4. **Correctness always beats performance** — don't sacrifice correctness
5. **Readability matters** — if two approaches are equally fast, prefer the readable one
