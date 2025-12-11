# Deployment Guide

## Option 1: Vercel CLI (Recommended if installed)
1. Open a terminal in this folder.
2. Run `vercel`.
3. Follow the prompts (Keep default settings).

## Option 2: Git + Vercel Dashboard
1. Initialize a git repository:
   ```bash
   git init
   git add .
   git commit -m "Initial commit"
   ```
2. Push to GitHub/GitLab/Bitbucket.
3. Go to [vercel.com](https://vercel.com).
4. Click **Add New Project**.
5. Import your repository.
6. Vercel will detect the settings automatically. Click **Deploy**.

## Note
The `vercel.json` file ensures that `index.html` is served correctly as a static site.
