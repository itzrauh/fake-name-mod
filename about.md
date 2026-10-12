# Fake Name

Displays a **local, client-side fake name** instead of your real one in Geometry Dash. It is purely cosmetic and only visible to you: it does not touch the servers or change your account.

## What it does

- Stores the fake name in a text setting.
- Replaces your name with the fake one everywhere the game draws it, matching your account ID:
  - Main menu (`MenuLayer`)
  - Profile page (`ProfilePage`)
  - Comments (`CommentCell`)
  - Leaderboards and user lists (`GJScoreCell`, `GJUserCell`)

The name label is re-centered precisely by measuring the real width of each glyph, so it stays aligned no matter how long the fake name is.

## Limitations

- Only you see it; everyone else still sees your real name.
- It does not change your account's real name.
- Can conflict with other mods that change these labels.
