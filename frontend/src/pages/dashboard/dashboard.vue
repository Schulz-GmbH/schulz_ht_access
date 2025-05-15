<template>
	<div class="flex flex-wrap -mx-3">
		<Widget title="WLAN" :isLoading="systemStore.loading" :icon="'fa-wifi'"
			:iconBackground="systemStore.wlan.connection.connected ? 'from-blue-700 to-blue-500' : 'from-gray-700 to-gray-500'"
			:skeletonRows="2" :skeletonCols="2" :skeletonWithIcon="true" :skeleton="false">
			<template #body>
				<span v-if="systemStore.wlan.connection.status">Aktiv&nbsp;</span>
				<span v-else>Deaktiviert&nbsp;</span>
				<span class="text-sm font-bold"
					:class="systemStore.wlan.connection.connected ? 'text-lime-500' : 'text-red-500'">
					{{ systemStore.wlan.connection.connected ? 'verbunden' : 'getrennt' }}
				</span>
			</template>
		</Widget>

		<!-- card2 -->
		<Widget title="App Version" :isLoading="systemStore.loading" :icon="'fa-code-branch'"
			:iconBackground="systemStore.version.web !== 'unbekannt' ? 'from-blue-700 to-blue-500' : 'from-gray-700 to-gray-500'"
			:skeletonRows="2" :skeletonCols="2" :skeletonWithIcon="true" :skeleton="false">
			<template #body>
				<span>{{ systemStore.version.web }}&nbsp;</span>
				<span v-if="systemStore.version.web !== 'unbekannt'"
					class="leading-normal text-sm font-weight-bolder text-lime-500">Up to
					Date</span>
			</template>
		</Widget>

		<!-- card3 Todo: if Debug true -->
		<Widget title="Erweiteres Logging" :isLoading="systemStore.loading" :icon="'fa-code'"
			:iconBackground="systemStore.logging.state ? 'from-blue-700 to-blue-500' : 'from-gray-700 to-gray-500'"
			:skeletonRows="2" :skeletonCols="2" :skeletonWithIcon="true" :skeleton="false">
			<template #body>
				<span>Status&nbsp;</span>
				<span class="leading-normal text-sm font-weight-bolder text-lime-500"
					:class="systemStore.logging.state ? 'text-lime-500' : 'text-red-500'">
					{{ systemStore.logging.state ? 'Aktiv' : 'Deaktiviert' }}</span>
			</template>
		</Widget>
	</div>
	<!-- Todo: Aus Storage laden -->
	<div class="flex flex-wrap my-6 -mx-3">

		<Table title="Last 6 Log Files" :data="logs" :columns="columns" :meta="meta" :params="params"
			:headerControl="true" :footerControl="false" @update:params="onParamsChange">
			<!-- Named slot "view" -->
			<template #view="{ row }">
				<button type="button" className="text-slate-700 mr-2 flex h-8 w-8 items-center justify-center"
					:title="`View ${row.filename}`" @click="viewLog(row.filename)">
					<i class="fas fa-eye"></i>
				</button>

			</template>

			<!-- Named slot "action" -->
			<template #action="{ row }">
				<button type="button" :title="`Delete ${row.filename}`"
					className="bg-gradient-to-tl from-red-700 to-red-500 text-white shadow-soft-2xl mr-2 flex h-8 w-8 items-center justify-center"
					@click="deleteLog(row.filename)">
					<i class="fas fa-trash-alt"></i>
				</button>
			</template>
		</Table>
	</div>
</template>

<script lang="ts">

export { default } from './dashboard'

</script>

<style src="./dashboard.scss"></style>
