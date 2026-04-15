# PES Version Control System

## Name: Shailaja PC  
## USN: PES1UG24AM449

---

## Phase 1: Object Store
- Implemented SHA-256 hashing using OpenSSL
- Stored objects in `.pes/objects/`
- Implemented deduplication
- Verified data integrity

---

## Phase 2: Tree Objects
- Implemented directory structure serialization
- Ensured deterministic tree creation
- Verified using test cases

---

## Phase 3: Index (Staging Area)
- Implemented `.pes/index`
- Added files using `pes add`
- Implemented status tracking

---

## Phase 4: Commit
- Implemented commit functionality
- Stored commit messages
- Cleared staging area after commit

---

## Phase 5: Log
- Implemented commit history
- Stored logs in `.pes/log`
- Displayed using `pes log`

---

## Analysis Questions

### Q5.1
Branching allows multiple lines of development without affecting the main code.

### Q5.2
Branches are lightweight and created by referencing existing commits.

### Q5.3
Merging combines branches and may require conflict resolution.

### Q6.1
Garbage collection removes unused objects to save space.

### Q6.2
It improves performance and keeps the repository efficient.

---

## Conclusion
Successfully implemented a simplified Git-like version control system with object storage, staging, commits, and logging.
