<template>
	<div class="flex flex-wrap -mx-3">
		<Widget title="WLAN" :isLoading="systemStore.wlan.loading" :icon="'fa-wifi'"
			:iconBackground="systemStore.wlan.connected ? 'from-blue-700 to-blue-500' : 'from-gray-700 to-gray-500'"
			:skeletonRows="2" :skeletonCols="2" :skeletonWithIcon="true" :skeleton="false">
			<template #body>
				<span v-if="systemStore.wlan.status">Aktiv&nbsp;</span>
				<span v-else>Deaktiviert&nbsp;</span>
				<span class="text-sm font-bold" :class="systemStore.wlan.connected ? 'text-lime-500' : 'text-red-500'">
					{{ systemStore.wlan.connected ? 'verbunden' : 'getrennt' }}
				</span>
			</template>
		</Widget>

		<!-- card2 -->
		<Widget title="App Version" :isLoading="systemStore.version.loading" :icon="'fa-code-branch'"
			:iconBackground="systemStore.version.value !== 'unbekannt' ? 'from-blue-700 to-blue-500' : 'from-gray-700 to-gray-500'"
			:skeletonRows="2" :skeletonCols="2" :skeletonWithIcon="true" :skeleton="false">
			<template #body>
				<span>{{ systemStore.version.value }}&nbsp;</span>
				<span v-if="systemStore.version.value !== 'unbekannt'"
					class="leading-normal text-sm font-weight-bolder text-lime-500">Up to
					Date</span>
			</template>
		</Widget>

		<!-- card3 Todo: if Debug true -->
		<Widget title="Erweiteres Logging" :isLoading="systemStore.logging.loading" :icon="'fa-code'"
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
			:headerControl="true" :footerControl="false" />
	</div>
</template>

<script lang="ts">

export { default } from './dashboard'

</script>

<style src="./dashboard.scss"></style>
