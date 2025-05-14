<template>
	<nav aria-label="Breadcrumb" role="navigation" class="breadcrumb">
		<ol class="flex flex-wrap pt-1 mr-12 bg-transparent rounded-lg sm:mr-16">
			<li class="breadcrumb-item leading-normal text-sm">
				<router-link :to="{ name: 'Dashboard' }" class="opacity-50 text-slate-700">
					Pages
				</router-link>
			</li>
			<li v-for="(item, index) in breadcrumbs" :key="item.to"
				class="breadcrumb-item text-sm pl-2 capitalize leading-normal text-slate-700 before:float-left before:pr-2 before:text-gray-600 before:content-['/']"
				:class="{ active: index === breadcrumbs.length - 1 }">
				<router-link v-if="index < breadcrumbs.length - 1" :to="item.to"
					class="breadcrumb-inner hover:underline">
					{{ item.text }}
				</router-link>
				<span v-else>{{ item.text }}</span>
			</li>
		</ol>

		<h6 class="mb-0 font-bold capitalize">
			{{ breadcrumbs[breadcrumbs.length - 1]?.text || '' }}
		</h6>
	</nav>
</template>

<script lang="ts">
export { default } from './Breadcrumb'

</script>

<style src="./Breadcrumb.scss"></style>
