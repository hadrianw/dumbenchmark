#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define INSERT_QUERY "INSERT INTO library VALUES (?, ?)"

static sqlite3 *db;
static sqlite3_stmt *insert_stmt;

static int
step(const char *path, const struct stat *sb, int flag, struct FTW *ftwbuf)
{
	int fd;
	char *fbuf;
	int rc;

	if(flag != FTW_F) {
		return 0;
	}

	if(sqlite3_reset(insert_stmt) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_reset failed: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	if(sqlite3_bind_text(insert_stmt, 1, path, strlen(path), SQLITE_TRANSIENT) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_bind_text path failed: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	fd = open(path, O_RDONLY);
	if(fd < 0) {
		fprintf(stderr, "open %s failed: %s\n", path, strerror(errno));
		return -1;
	}
	fbuf = mmap(NULL, sb->st_size, PROT_READ, MAP_SHARED, fd, 0);
	if(fbuf == NULL) {
		fprintf(stderr, "mmap %s failed: %s\n", path, strerror(errno));
		return -1;
	}
	rc = sqlite3_bind_text(insert_stmt, 2, fbuf, sb->st_size, SQLITE_TRANSIENT);
	munmap(fbuf, sb->st_size);
	close(fd);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "sqlite3_bind_text body failed: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	if(sqlite3_step(insert_stmt) != SQLITE_DONE) {
		fprintf(stderr, "sqlite3_step failed: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	int rc = -1;
	char *err = 0;
	
	if(sqlite3_open(argv[1], &db) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
		goto out_close_db;
	}
	
	if(sqlite3_exec(db, "CREATE VIRTUAL TABLE library USING fts5(path, content)", NULL, NULL, &err) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_exec failed: %s\n", err);
		sqlite3_free(err);
		goto out_close_db;
	}

	if(sqlite3_prepare_v2(db, INSERT_QUERY, sizeof(INSERT_QUERY), &insert_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_prepare_v2 failed: %s\n", sqlite3_errmsg(db));
		goto out_close_db;
	}
	
	rc = nftw(argv[2], step, 16, 0);
	if(rc) {
		fprintf(stderr, "ntfw failed: %s\n", strerror(errno));
	}

	sqlite3_finalize(insert_stmt);
out_close_db:
	sqlite3_close(db);
	return rc;
}
