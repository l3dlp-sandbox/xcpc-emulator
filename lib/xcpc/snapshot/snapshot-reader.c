/*
 * snapshot-reader.c - Copyright (c) 2001-2022 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snapshot-reader-priv.h"

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcSnapshotReader::%s()", function);
}

XcpcSnapshotReader* xcpc_snapshot_reader_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcSnapshotReader);
}

XcpcSnapshotReader* xcpc_snapshot_reader_free(XcpcSnapshotReader* self)
{
    log_trace("free");

    return xcpc_delete(XcpcSnapshotReader, self);
}

XcpcSnapshotReader* xcpc_snapshot_reader_construct(XcpcSnapshotReader* self, XcpcSnapshot* snapshot)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(self, 0, sizeof(XcpcSnapshotReader));
    }
    /* initialize members */ {
        self->status   = XCPC_SNAPSHOT_STATUS_SUCCESS;
        self->snapshot = snapshot;
        self->file     = NULL;
    }
    return self;
}

XcpcSnapshotReader* xcpc_snapshot_reader_destruct(XcpcSnapshotReader* self)
{
    log_trace("destruct");

    return self;
}

XcpcSnapshotReader* xcpc_snapshot_reader_new(XcpcSnapshot* snapshot)
{
    log_trace("new");

    return xcpc_snapshot_reader_construct(xcpc_snapshot_reader_alloc(), snapshot);
}

XcpcSnapshotReader* xcpc_snapshot_reader_delete(XcpcSnapshotReader* self)
{
    log_trace("delete");

    return xcpc_snapshot_reader_free(xcpc_snapshot_reader_destruct(self));
}

static XcpcSnapshotStatus xcpc_snapshot_reader_load_header(XcpcSnapshotReader* self)
{
    log_trace("load_header");
    /* load header */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            void*  header_data = &self->snapshot->header;
            size_t header_size = sizeof(self->snapshot->header);
            size_t byte_count  = fread(header_data, 1, header_size, self->file);
            if(byte_count != header_size) {
                self->status = XCPC_SNAPSHOT_STATUS_HEADER_ERROR;
            }
        }
    }
    return self->status;
}

static XcpcSnapshotStatus xcpc_snapshot_reader_load_memory(XcpcSnapshotReader* self)
{
    size_t remaining_bytes = 0;

    log_trace("load_memory");
    /* compute and check remaining memory size */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            remaining_bytes |= (((size_t)(self->snapshot->header.ram_size_h)) << 18);
            remaining_bytes |= (((size_t)(self->snapshot->header.ram_size_l)) << 10);
            if(remaining_bytes == 0) {
                self->status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
            }
        }
    }
    /* load memory banks */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            unsigned int bank_index = 0;
            unsigned int bank_count = countof(self->snapshot->memory);
            for(bank_index = 0; bank_index < bank_count; ++bank_index) {
                void*  bank_data = self->snapshot->memory[bank_index].data;
                size_t bank_size = sizeof(self->snapshot->memory[bank_index].data);
                if(remaining_bytes >= bank_size) {
                    size_t byte_count = fread(bank_data, 1, bank_size, self->file);
                    if(byte_count != bank_size) {
                        self->status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
                        break;
                    }
                    else {
                        remaining_bytes -= bank_size;
                    }
                }
                else {
                    break;
                }
            }
        }
    }
    /* check remaining memory size */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            if(remaining_bytes != 0) {
                self->status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
            }
        }
    }
    return self->status;
}

XcpcSnapshotStatus xcpc_snapshot_reader_load(XcpcSnapshotReader* self, const char* filename)
{
    log_trace("load");

    /* sanity check */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            self->status = xcpc_snapshot_sanity_check(self->snapshot);
        }
    }
    /* check filename */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            if((filename == NULL) || (*filename == '\0')) {
                self->status = XCPC_SNAPSHOT_STATUS_BAD_FILENAME;
            }
        }
    }
    /* open file */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            self->file = fopen(filename, "r");
            if(self->file == NULL) {
                self->status = XCPC_SNAPSHOT_STATUS_FILE_ERROR;
            }
        }
    }
    /* load header */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            self->status = xcpc_snapshot_reader_load_header(self);
        }
    }
    /* load memory */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            self->status = xcpc_snapshot_reader_load_memory(self);
        }
    }
    /* close file */ {
        if(self->file != NULL) {
            self->file = ((void) fclose(self->file), NULL);
        }
    }
    /* sanity check */ {
        if(self->status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            self->status = xcpc_snapshot_sanity_check(self->snapshot);
        }
    }
    return self->status;
}
