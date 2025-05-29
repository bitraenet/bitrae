# Bitrae Core v0.2.1 Release Notes

## 📦 Introduction

Bitrae Core version **0.2.1** is now available for download:

🔗 [Release v0.2.1 on GitHub](https://github.com/bitraenet/bitrae/releases/tag/v0.2.1)

This is a **minor maintenance release** that includes important **data updates, sync optimizations**, and **installation fixes**. While not security-critical, it is **recommended for all users** to upgrade to benefit from improved stability and setup enhancements.

Please report any issues via the GitHub issue tracker:  
🔗 [Bitrae Issue Tracker](https://github.com/bitraenet/bitrae/issues)

---

## 🔄 Notable Changes

### ⛓️ Chain Data & Consensus Updates

- **Updated `checkpointData` and `chainTxData`**  
  - **Commit:** [`8a09ec4`](https://github.com/bitraenet/bitrae/commit/8a09ec413093ee1af60dfc22bb17ac92438bf4b8)  
  - Adds checkpoints up to **block 595000** to improve initial block download (IBD) speed and syncing accuracy.
  - Updates transaction data for improved syncing performance.

- **Updated Consensus Parameters**  
  - **Commit:** [`ca7ba67`](https://github.com/bitraenet/bitrae/commit/ca7ba679148846e550683749840ecf14b13c1f15)  
  - Sets **default assume-valid block** to **block 550000** for faster node synchronization.
  - Updates total chainwork values to reflect current network progress.

---

### 🛠️ Build & Installation Improvements

- **Fixed BerkeleyDB Build Script Redirects**  
  - **Commit:** [`01ab245`](https://github.com/bitraenet/bitrae/commit/01ab24533fb9e12ac2075c9e8eaf82b561a813a7)  
  - Updated URLs for `config.guess` and `config.sub` to resolve **HTTP 302 redirect issues** during BerkeleyDB compilation.

- **Improved Build Script UX for Git Cloning**  
  - **Commit:** [`928ed46`](https://github.com/bitraenet/bitrae/commit/928ed467bffe6d5481799891ffd9c4bd54968135)  
  - Adds **user prompt to choose** between cloning the **latest release (recommended)** or the **main development branch**.
  - Includes **8-second countdown** fallback to default behavior.
  - Enhances error handling for release tag retrieval and fallback logic.

---

### 🧾 General Updates

- **Version Bump and Metadata Update**  
  - **Commits:**  
    - [`4a8b5a8`](https://github.com/bitraenet/bitrae/commit/4a8b5a8ad098eb045cf099e093ef95e4a4c5cf05) – Set project version to **v0.2.1**, updated copyright
    - [`f585a19`](https://github.com/bitraenet/bitrae/commit/f585a19a78cf1bc10e04a6bce05547029a582e20) – Updated copyright
  - Updates official versioning and prepares the repository for the 2025 cycle.

---

## 🚀 Upgrade Now

We recommend that all **users, miners, and node operators** upgrade to **Bitrae Core v0.2.1** to stay current with the latest improvements and chain data.

📥 **Download Bitrae Core v0.2.1:**  
🔗 [GitHub Releases](https://github.com/bitraenet/bitrae/releases/tag/v0.2.1)

---

## 📝 Notes for Users

- This release includes no consensus or RPC-breaking changes.
- Pruned nodes benefit from the updated assume-valid settings, reducing unnecessary validation of historical blocks.
- Developers using build scripts should now experience smoother setup with updated dependency URLs and improved clone handling.

---

## ✅ Summary of Key Changes

✔ Updated checkpoints and transaction stats  
✔ Enhanced syncing performance  
✔ Fixed 302 redirect issue for BerkeleyDB installs  
✔ Added user input and fallback logic to clone script  
✔ Minor metadata updates and version bump  

---

### 🔗 Report Issues

If you experience any issues, please open a ticket on our GitHub issue tracker:  
🔗 [Bitrae Issues](https://github.com/bitraenet/bitrae/issues)

Thank you for supporting **Bitrae Core**! 🚀
