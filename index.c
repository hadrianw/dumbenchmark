#include <ftw.h>
#include <stdio.h>
#include <sqlite3.h>

#define INSERT_QUERY "INSERT INTO kernel VALUES (?, ?)", 

int step(const char *path, const struct stat *sb, int flag, struct FTW *ftwbuf)
{
	// sqlite3_bind_text
	// sqlite3_bind_text
	// sqlite3_step
	// sqlite3_reset
}

int main(int argc, char *argv[])
{
	int rc = -1;
	sqlite3 *db;
	sqlite3_stmt *insert_stmt;
	char *err = 0;
	DIR *dp;
	struct dirent *ep;
	
	if(sqlite3_open(argv[1], &db)) {
		fprintf(stderr, "sqlite3_open failed: %s\n", sqlite3_errmsg(db));
		goto out_close_db;
	}
	
	if(sqlite3_exec(db, "CREATE VIRTUAL TABLE kernel USING fts5(path, content)", NULL, NULL, &err) != SQLITE_OK) {
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