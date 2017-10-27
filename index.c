#include <ftw.h>
#include <stdio.h>
#include <sqlite3.h>

#define INSERT_QUERY "INSERT INTO index VALUES (?, ?)", 

static sqlite3 *db;
static sqlite3_stmt *insert_stmt;

static int step(const char *path, const struct stat *sb, int flag, struct FTW *ftwbuf)
{
	int fd;
	char *fbuf;

	if(flag != FTW_F) {
		return 0;
	}

	sqlite3_bind_text(insert_stmt, 1, path, strlen(path), SQLITE_TRANSIENT);

	fd = open(path, O_RDONLY);
	fbuf = mmap(NULL, sb->st_size, PROT_READ, MAP_SHARED, fd, 0);
	sqlite3_bind_text(insert_stmt, 2, fbuf, sb->st_size, SQLITE_TRANSIENT);
	munmap(fbuf, sb->st_size);

	// sqlite3_step
	// sqlite3_reset
}

int main(int argc, char *argv[])
{
	int rc = -1;
	char *err = 0;
	
	if(sqlite3_open(argv[1], &db)) {
		fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
		goto out_close_db;
	}
	
	if(sqlite3_exec(db, "CREATE VIRTUAL TABLE index USING fts5(path, content)", NULL, NULL, &err) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_exec failed: %s\n", err);
		sqlite3_free(err);
		goto out_close_db;
	}

	if(sqlite3_prepare_v2(db, INSERT_QUERY, sizeof(INSERT_QUERY), &insert_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "sqlite3_prepare_v2 failed: %s\n", sqlite3_errmsg(db));
		goto out_close_db;
	}
	
	nftw(argv[2], step, 16, 0);

	rc = 0;	
out_close_db:
	sqlite3_close(db);
	return rc;
}