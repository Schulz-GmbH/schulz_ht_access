<template>
	<transition enter-active-class="transition-transform duration-300 ease-out" enter-from-class="-translate-x-full"
		enter-to-class="translate-x-0" leave-active-class="transition-transform duration-300 ease-in"
		leave-from-class="translate-x-0" leave-to-class="-translate-x-full">
		<aside v-show="!isCollapse" @click="onDevClick" class="max-w-62.5 ease-nav-brand fixed inset-y-0 my-4 ml-4 block w-full flex-wrap items-center justify-between
			overflow-y-auto rounded-2xl border-0 bg-white p-0 antialiased transition-transform
			duration-200 xl:left-0 xl:translate-x-0 xl:bg-transparent z-[999]">
			<div class="h-auto">
				<i class="absolute top-0 right-0 p-4 opacity-50 cursor-pointer text-slate-400 xl:hidden"></i>
				<router-link :to="{ name: 'Dashboard' }"
					class="flex justify-center m-0 px-8 py-4 text-lg text-slate-700 whitespace-nowrap">
					<span class="ml-1 font-semibold transition-all duration-200 ease-nav-brand">
						{{ appTitle }}
					</span>
				</router-link>
			</div>
			<hr class="h-px mt-0 mb-4 bg-transparent bg-gradient-to-r ">
			<div class="items-center block w-auto max-h-screen overflow-auto h-sidenav grow basis-full">
				<ul class="flex flex-col pl-0 mb-0">
					<template v-for="block in sortedRoutes" :key="block.group || 'ungrouped'">
						<li v-if="block.group" class="px-4 py-1 font-semibold text-slate-500">
							{{ $t(block.group) }}
						</li>

						<li v-for="routeObj in block.items" :key="routeObj.path" class="mt-0.5 w-full">
							<router-link :to="routeObj.path" :class="linkClasses(routeObj)">
								<div :class="iconWrapperClasses(routeObj)"
									class="shadow-soft-2xl mr-2 flex h-8 w-8 items-center justify-center rounded-lg bg-white">
									<i :class="routeObj.meta!.icon"></i>
								</div>
								<span class="ml-1 duration-300 opacity-100 pointer-events-none ease-soft">
									{{ $t(routeObj.meta!.title as string) }}
									<small v-if="routeObj.meta?.dev || routeObj.meta?.hidden"
										class="text-xs text-red-500 ml-1">
										{{ $t('components.sidebar.devBadge') }}
									</small>
								</span>
							</router-link>
						</li>
					</template>
				</ul>
			</div>
		</aside>
	</transition>
</template>

<script lang="ts">

export { default } from './sidebar'

</script>

<style src="./sidebar.scss"></style>
