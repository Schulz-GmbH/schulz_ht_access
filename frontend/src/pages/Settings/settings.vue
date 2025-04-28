<template>
	<div class="flex flex-wrap mb-12 -mx-3">
		<div class="w-full max-w-full px-3 lg:flex-0 shrink-0 lg:w-3/12"></div>
		<div class="w-full max-w-full px-3 shrink-0 lg:w-9/12">
			<div class="relative flex flex-col min-w-0 mt-6 break-words bg-white border-0 shadow-soft-xl rounded-2xl bg-clip-border"
				id="settings">
				<div class="p-6 rounded-t-2xl">
					<h5>Einstellungen</h5>
					<p class="leading-normal text-sm">
						Choose how you receive notifications. These notification settings
						apply to the things you’re watching.
					</p>
				</div>
				<div class="flex-auto p-6 pt-0">
					<div class="overflow-x-auto">
						<Table id="settingsTable" :columns="columns" :data="rows" :meta="tableMeta"
							:params="tableParams" :loading="tableLoading" :headerControl="false" :footerControl="false"
							:shadow="false">
							<!-- Slot für die Activity-Spalte -->
							<template #activity="{ row }">
								<div class="flex flex-col">
									<!-- Haupt-Zeile -->
									<div class="my-auto">
										<span class="block leading-normal text-sm text-slate-700">
											{{ row.activity.title }}
										</span>
										<span class="font-normal leading-tight text-xs whitespace-pre-line">
											{{ row.activity.subtitle }}
										</span>
									</div>
								</div>
							</template>

							<!-- Slot für die state-Checkbox -->
							<template #state="{ row }">
								<div class="flex items-center justify-center pl-12 mb-0 min-h-6">
									<input type="checkbox" :checked="row.state"
										@change="onToggleCheckbox(row, 'state', $event)"
										class="align-top appearance-none float-left relative mt-0.5 -ml-12 h-5 w-10 border border-solid border-gray-200 checked:border-slate-800/95 bg-none bg-contain bg-left bg-no-repeat bg-slate-800/10 checked:bg-none checked:bg-right checked:bg-slate-800/95 rounded-10 cursor-pointer transition-all duration-250 ease-soft-in-out after:absolute after:content-[''] after:duration-250 after:h-4 after:w-4 after:rounded-circle after:shadow-soft-2xl after:top-px after:translate-x-px after:bg-white checked:after:translate-x-5.3" />
								</div>
							</template>
						</Table>
					</div>
				</div>
			</div>
		</div>
		<div class="w-full max-w-full px-3 shrink-0 lg:w-9/12">
			<Transition enter-active-class="transition ease-out duration-300"
				enter-from-class="opacity-0 transform -translate-y-2"
				enter-to-class="opacity-100 transform translate-y-0"
				leave-active-class="transition ease-in duration-200"
				leave-from-class="opacity-100 transform translate-y-0"
				leave-to-class="opacity-0 transform -translate-y-2">
				<div class="flex flex-wrap my-6 -mx-3" v-if="systemStore.wlan.status">
					<Table id="network" title="WLan-Network" :data="networkRows" :columns="networkColumns"
						:meta="networkMeta" :params="networkParams" :headerControl="true" :footerControl="false" />
				</div>
			</Transition>
		</div>
	</div>
</template>

<script lang="ts">
import settings from './settings';


export { default } from './settings'

</script>

<style src="./settings.scss" scoped></style>
