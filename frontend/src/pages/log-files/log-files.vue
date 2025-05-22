<template>
	<div class="p-2 space-y-6" id="wlan">
		<!-- Header -->
		<div class="space-y-1 text-xs">
			<div class="flex items-center justify-center mb-2">
				<div class="flex justify-center items-center w-12 h-12 rounded-xl bg-slate-800/10 p-3 text-slate-700">
					<i class="text-xl fas fa-file-alt text-slate-700"></i>
				</div>
			</div>
			<h2 class="text-2xl font-semibold text-center">
				{{ $t('pages.log-file.header.log.title') }}
			</h2>
			<p class="text-center text-slate-500">
				{{ $t('pages.log-file.header.log.description') }}
			</p>
		</div>

		<!-- Toggle WLAN On/Off -->
		<div class="bg-white rounded-2xl p-4 shadow flex flex-col items-center justify-between space-y-2 text-xs">
			<div class="relative flex w-full items-center justify-between border border-gray-300 rounded-full">
				<i class="fas fa-search absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400"></i>
				<label class="relative inline-block w-full">
					<input v-model="query" type="search" :placeholder="$t('pages.log-file.search.placeholder')"
						class="w-full pl-10 p-2 rounded-full focus:outline-none placeholder-gray-400" />
				</label>
			</div>
		</div>
	</div>

	<!-- Log-Files -->
	<div class="p-2 space-y-6">
		<div class="font-semibold text-slate-500 uppercase px-2 mb-2">
			{{ $t('pages.log-file.header.log.listHeader') }}
		</div>
		<ul class="text-xs bg-white rounded-2xl shadow divide-y divide-slate-200">
			<li v-if="filteredLogs.length" v-for="row in filteredLogs" :key="row.filename" style="touch-action: pan-y;"
				@touchstart="onTouchStart($event, row.filename)" @touchmove="onTouchMove($event, row.filename)"
				@touchend="onTouchEnd($event, row.filename)" :ref="setItemRef(row.filename)"
				@click="viewLog(row.filename)"
				class="relative flex items-center justify-between px-4 py-3 hover:bg-slate-50 cursor-pointer">


				<div class="absolute inset-y-0 right-0 flex text-slate-700 opacity-1 transition-opacity duration-200"
					style="z-index: 99999;" :ref="setBtnRef(row.filename)">
					<button @click.stop="openRename(row.filename)"
						class="w-16 bg-white flex items-center justify-center">
						<i class="fas fa-comment-dots"></i>
					</button>
					<button @click.stop="shareLog(row.filename)" class="w-16 bg-white flex items-center justify-center">
						<i class="fas fa-paper-plane"></i>
					</button>
					<button @click.stop="openDelete(row.filename)"
						class="w-16 bg-white flex items-center justify-center rounded-r-full">
						<i class="fas fa-trash-alt"></i>
					</button>
				</div>

				<div class="h-4 w-4 flex-none flex justify-center items-center space-x-3">
					<i class="fa-solid text-blue-400"></i>
				</div>
				<span class="flex-1 truncate mx-4">{{ row.filename }}</span>
				<div class="flex flex-none items-center space-x-3" @click="viewLog(row.filename)">
					<i class="fas fa-info-circle text-blue-500"></i>
				</div>
			</li>
			<li v-else class="px-4 py-3 text-center text-slate-400">
				{{ $t('pages.log-file.empty') }}
			</li>
		</ul>
	</div>


	<!-- Modal Umbenennen -->
	<Modal v-model="renameModalVisible" @confirm="doRename" @close="renameModalVisible = false">
		<template #header>{{ $t('pages.log-file.modal.renameHeader') }}</template>
		<template #default>
			<input v-model="newFilename" type="text" class="w-full border rounded px-2 py-1"
				:placeholder="$t('pages.log-file.modal.renamePlaceholder')" />
		</template>
	</Modal>

	<!-- Modal Löschen -->
	<Modal v-model="deleteModalVisible" @confirm="doDelete" @close="deleteModalVisible = false">
		<template #header>{{ $t('pages.log-file.modal.deleteHeader') }}</template>
		<template #default>
			{{ $t('pages.log-file.modal.deleteBody', { filename: pendingFilename }) }}
		</template>
	</Modal>
</template>

<script lang="ts">
export { default } from "./log-files";
</script>

<style lang="scss" src="./log-files.scss" scoped></style>
