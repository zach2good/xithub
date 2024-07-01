## Tech Stack

- Database: MariaDB
- Backend Services: General Python
- Backend Endpoints: Python + Flask?
- Frontend: Something something React?

## Prerelease

- Use TCP instead of UDP? We really don't want dropped packets
- Make sure we use the same compression everywhere (GZip, zlib, ZipFile, Brotli, etc.).
- Handle both S2C and C2S packets.
- Track current target name+id for every C2S packet?
- Timed fallback for sending buffer from client, a few seconds?.
- Forced send from client on logout.
- Unify on UserId or CharacterToken.
- Client remembers your token/id.
- Do not archive packets that are part of a currently-ongoing session.
- Cut session if character name changes (logging into alts), but session should be cut automatically on logout.
- Allow users to manually cut sessions (inject a special marker packet).
- A final stage of processing to happen when a session is closed.
- OAuth (Google only) authentication flow, server + client
- Premade/cached lookups per session (generated during processing):
  - Events + Args
  - Mobs spawned
  - Estimate mob HP
  - Entities encountered (names + ids + types)
  - Zones encountered
- Dashboards for this sort of information?
- Lock in packet formats, binary formats, and db schema for packets.
- Confirm that private servers won't pollute the data.
- Address all leftover code/design TODOs

## Closed testing (<20 people)

- Measure the load per-user in terms of processing time and database size.
- Write up terms and conditions - you won't be allowed to revoke your packet data!
- GDPR, User Data, Safety
- Make sure that your IP is checked when packets with your token come in, to help make sure you can't be impersonated.
- Maybe use signing keys for this?
- What packets _shouldn't_ we log, or we should hide parts of? (C2S 0x00A, Chat messages, etc.)
- Host server somewhere in GCP, project cost per month.
- Donation button.
- Admin screen to manage/remove users/data etc.
- User screen to annotate their data.
- Cron jobs to make sure services never go down, or handle as part of docker-compose.
- git push to build and deploy pipeline, including rollback and database snapshot and rollback.
- Reimport packet data from archives based on date - truncating existing data in DB if needed.
- Maybe a month of testing here?

## Beta (<100 people)

- Users sign up with Google OAuth and get told they're on a wait list. Then they'll get manually approved and will get their code in an email or on their account page.
- Make sure there are systems in place to make sure bad actors can't flood the server with large amounts of data, or bunk data.
  - Data comes in with a UserToken, will be checked against the IP we have on file. If they don't match, discard.
  - This doesn't seem good now that I think about it. We might need signing keys to account for people with dynamic IPs.
- Admin flag for people submitting from multiple/new IPs
- Roles for users: Admin / Submitter / Editor / Viewer, etc.
- Monitoring dashboards for:
  - Input packet traffic / General network throughput
  - Number of active sessions
  - Packet processing backlog
  - Packet archival backlog
  - Database Size
  - Database Query latency
  - Most common queries
- Word of mouth only

## GA (Unlimited people)

- Look into database optimisation (if needed)
- Advertising in Reddit/Discord

## After GA

- Allow users to look up their sessions and add links to videos to them
  - Would be lovely to embed them too (but not host!).
- Allow users to download their session packet data, in multiple formats (PacketViewer, Binary, etc.)
- Archive to GCP's Cold Storage equivalent
- AH data?
- What if it gets expensive to run/maintain?
- Allow users to upload zips/txt files of their existing cap collections and have them be processed (Doesn't seem too hard)
- Generalise everything to not be specific to FFXI data, but for (BinaryData + {key:value, key:value, key:value}, etc.)
- Will MariaDB be able to scale over time without truncating data? Do we need to consider other DBs, or even our own file/index/crawler system?
